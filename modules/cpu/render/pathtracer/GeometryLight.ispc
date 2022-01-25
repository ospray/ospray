// Copyright 2009-2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "render/pathtracer/GeometryLight.ih"
#include "render/pathtracer/PathStructs.ih"
#include "render/pathtracer/PathTracerUtil.ih"

#include "render/Material.ih"

#include "geometry/GeometricModel.ih"
#include "lights/Light.ih"
#include "math/AffineSpace.ih"
#include "math/Distribution1D.ih"

vec3f evaluateGeometryLights(const PathContext &pathContext,
    const PathState &pathState,
    const PathVertex &lastVertex,
    const PathVertex &pathVertex,
    Ray &ray)
{
  vec3f L = make_vec3f(0.f);
  Material *material = (Material *)pathVertex.dg.material;
  foreach_unique (m in material) {
    if (m != NULL && reduce_max(m->emission) > 0.f) {
      // at the moment we only support uniform light selection
      // therefore we can evaluate the selection PDF directly
      float lightSelectionPDF = 0.0f;
      if (pathContext.numLights > 0) {
        lightSelectionPDF =
            (1.0f / (float)pathContext.numLights) * pathContext.numLightSamples;
      }
      // convert pdf wrt. area to pdf wrt. solid angle
      const float cosd = dot(pathVertex.dg.Ng, ray.dir);
      const float lePdf = pathVertex.dg.areaPDF * sqr(ray.t) / abs(cosd);
      L = L
          + pathState.throughput * m->emission
              * misHeuristic(
                  pathState, lastVertex.pdf_w, lightSelectionPDF * lePdf);
    }
  }
  return L;
}

/* The GeometryLight is a proxy object fulfilling the Light.ih API.
   It is generated internally for each emitting geometry modelance to facilitate
   explicit importance sampling (currently wrt. the surface area).
*/

struct GeometryLight
{
  Light super; // inherited light fields

  GeometricModel *model; // underlying geometry
  int32 numPrimitives; // number of emissive primitives
  int32 *primIDs; // IDs of emissive primitives to sample
  float *distribution; // pdf over prims proportional to (world-space) area
  float pdf; // probability density to sample point on surface := 1/area
};

// Implementation /////////////////////////////////////////////////////////////

Light_SampleRes GeometryLight_sample(const Light *uniform super,
    const DifferentialGeometry &dg,
    const vec2f &s,
    const float time)
{
  const GeometryLight *uniform self = (GeometryLight * uniform) super;
  const GeometricModel *uniform model = self->model;
  const Geometry *uniform geo = model->geom;
  Light_SampleRes res;

  // choose primitive
  const Sample1D sample =
      Distribution1D_sample(self->numPrimitives, self->distribution, 0, s.x);
  const int32 primID = self->primIDs[sample.idx];

  // XXX have to rescale s.x
  const vec2f ns = make_vec2f(sample.frac, s.y);
  // sample position on primitive
  // TODO maybe should already be done wrt. solid angle
  SampleAreaRes as;
  const Instance *uniform instance = self->super.instance;
  assert(instance);
  if (instance->motionBlur)
    foreach_unique (utime in time) {
      const uniform affine3f xfm = Instance_getTransform(instance, utime);
      as = geo->sampleArea(geo, primID, xfm, rcp(xfm), ns, time);
    }
  else {
    as = geo->sampleArea(
        geo, primID, instance->xfm, instance->rcp_xfm, ns, time);
  }
  // note that sample.pdf/primitives * sampleArea(worldspace).pdf == self->pdf

  // extant light vector from the hit point
  const vec3f dir = as.pos - dg.P;
  const float dist = length(dir);

  // normalized light vector
  res.dir = dir / dist;
  res.dist = dist - calcEpsilon(as.pos, dist);

  // convert to pdf wrt. solid angle
  const float cosd = dot(as.normal, res.dir);
  res.pdf = self->pdf * sqr(dist) / abs(cosd);

  // get radiance
  Material *mat =
      (Material *)GeometricModel_getMaterial(model, dg.renderer, primID);
  const vec3f radiance = mat ? mat->emission : make_vec3f(0.f);

  res.weight = radiance * rcp(res.pdf);

  return res;
}

// Exports (called from C++) //////////////////////////////////////////////////

export bool uniform GeometryLight_isSupported(const void *uniform _model)
{
  GeometricModel *uniform model = (GeometricModel * uniform) _model;
  const Geometry *uniform geo = model->geom;
  return geo->sampleArea && geo->getAreas;
}

export void *uniform GeometryLight_create(
    void *uniform _model, void *uniform _renderer, void *uniform _instance)
{
  GeometricModel *uniform model = (GeometricModel * uniform) _model;

  if (!GeometryLight_isSupported(model)) {
    // set pdf to zero, indicating that this emissive geometry cannot be
    // sampled, but only be hit (and thus MIS weight for BSDF samples will be 1)
    model->areaPDF = 0.f;
    return NULL;
  }

  GeometryLight *uniform self = uniform new GeometryLight;
  const Renderer *uniform renderer = (Renderer * uniform) _renderer;
  const Geometry *uniform geo = model->geom;
  const Instance *uniform instance = (Instance * uniform) _instance;

  Light_Constructor(&self->super);
  self->super.sample = GeometryLight_sample;
  self->super.eval = NULL; // geometry lights are hit and explicitly handled
  self->super.instance = instance;
  self->model = model;

  // create the list of emissive primitive IDs
  self->primIDs = uniform new uniform int32[geo->numPrimitives];
  uniform int32 numEmissivePrims = 0;
  foreach (primID = 0 ... geo->numPrimitives) {
    Material *mat =
        (Material *)GeometricModel_getMaterial(model, renderer, primID);
    const vec3f emission = mat->emission;
    if (reduce_max(emission) > 0.f) {
      int offset = exclusive_scan_add(1);

      self->primIDs[numEmissivePrims + offset] = primID;
      numEmissivePrims += reduce_add(1);
    }
  }

  if (numEmissivePrims == 0) {
    // no emissive primitives, return NULL
    delete[] self->primIDs;
    model->areaPDF = 0.f;
    return NULL;
  } else if (numEmissivePrims < geo->numPrimitives) {
    // save memory
    int32 *uniform newPrimIDs = uniform new uniform int32[numEmissivePrims];
    memcpy(newPrimIDs, self->primIDs, numEmissivePrims * sizeof(uniform int32));
    delete[] self->primIDs;
    self->primIDs = newPrimIDs;
  }

  self->numPrimitives = numEmissivePrims;

  // create the sampling distribution
  // TODO: use emissive power instead of just area
  // TODO: motion blur can introduce scale, which means areas cannot accurately
  // be precomputed
  self->distribution = uniform new uniform float[numEmissivePrims];
  geo->getAreas(
      geo, self->primIDs, numEmissivePrims, instance->xfm, self->distribution);
  self->pdf = 1.f / Distribution1D_create(numEmissivePrims, self->distribution);
  model->areaPDF = self->pdf;

  return self;
}

void GeometryLight_destroy(void *uniform _self)
{
  GeometryLight *uniform self = (GeometryLight * uniform) _self;
  if (self) {
    delete[] self->primIDs;
    delete[] self->distribution;
  }
  delete self;
}