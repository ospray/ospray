// Copyright 2009 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "math/random.ih"
#include "math/sampling.ih"
#include "render/util.ih"
#include "surfaces.ih"
// c++ shared
#include "SciVisDataShared.h"
#include "lights/LightShared.h"
#include "lights/LightType.ih"

OSPRAY_BEGIN_ISPC_NAMESPACE

vec3f directIllumination(const uniform SciVis *uniform self,
    const FrameBuffer *uniform fb,
    const World *uniform world,
    const varying DifferentialGeometry &dg,
    ScreenSample &sample,
    varying LDSampler *uniform ldSampler,
    const varying SciVisBSDF &bsdf,
    const varying vec3f &inDir)
{
  vec3f color = make_vec3f(0.f);

  const uniform SciVisData &scivisData =
      *((const uniform SciVisData *)world->scivisData);
  if (!scivisData.lights)
    return color;

  // calculate shading for all lights
  for (uniform int i = scivisData.numLightsVisibleOnly;
       i < scivisData.numLights;
       i++) {
    const Light *uniform l = scivisData.lights[i];
    const vec2f s = make_vec2f(0.0f); // sample center of area lights

    const Light_SampleRes light =
        Light_dispatch_sample(l, dg, s, sample.ray.time);

    if (reduce_max(light.weight) > 0.f) { // any potential contribution?
      const float cosNL = dot(light.dir, dg.Ns);
      if (cosNL < 0.0f)
        continue;

      const vec3f H = normalize(light.dir - inDir);
      const float cosNH = dot(H, dg.Ns);
      const vec3f specular = bsdf.specular * powf(cosNH, bsdf.shininess);
      const vec3f diffuse = bsdf.diffuse * cosNL;
      const vec3f light_contrib = (diffuse + specular) * light.weight;

      if (self->shadowsEnabled) {
        if (luminance(light_contrib) > self->super.minContribution) {
          vec3f P = dg.P;
          if (dot(dg.Ng, light.dir) < 0.0f)
            P = P - (2.f * dg.epsilon) * dg.Ng;

          Ray shadowRay;
          setRay(shadowRay, P, light.dir, 0.0f, light.dist);

          vec3f light_alpha = lightAlpha(self,
              shadowRay,
              fb,
              world,
              sample,
              ldSampler,
              light_contrib,
              dg.epsilon,
              0.25f);

          color = color + light_alpha * light_contrib;
        }
      } else {
        color = color + light_contrib;
      }
    }
  }

  return color;
}

SYCL_EXTERNAL SSI SciVis_computeShading(const SciVis *uniform self,
    const FrameBuffer *uniform fb,
    const World *uniform world,
    const DifferentialGeometry &dg,
    ScreenSample &sample,
    varying LDSampler *uniform ldSampler,
    const varying vec3f &inDir)
{
  SSI retval;

  const SciVisBSDF bsdf = evalMaterial(dg);
  retval.albedo = bsdf.albedo;

  vec3f color =
      directIllumination(self, fb, world, dg, sample, ldSampler, bsdf, inDir);

  vec3f ao = make_vec3f(1.f);
  const uniform SciVisData &scivisData =
      *((const uniform SciVisData *)world->scivisData);
  if (self->aoSamples > 0
      && luminance(scivisData.aoColorPi) > self->super.minContribution)
    ao = SciVis_computeAO(self,
        fb,
        world,
        sample,
        ldSampler,
        dg,
        self->aoSamples,
        self->aoRadius,
        sample.sampleID);

  color = color + bsdf.diffuse * ao * scivisData.aoColorPi;

  retval.shadedColor = color;
  retval.transmission = bsdf.transmission;
  retval.opacity = bsdf.opacity;

  return retval;
}

SYCL_EXTERNAL vec3f evaluateLights(
    const World *uniform world, const DifferentialGeometry &dg, const Ray &ray)
{
  // Iterate through all lights
  vec3f color = make_vec3f(0.f);
  /*
  const uniform SciVisData &scivisData =
      *((const uniform SciVisData *)world->scivisData);
  for (uniform int i = 0; i < scivisData.numLights; i++) {
    // Skip lights with disabled visibility via parameter
    const Light *uniform l = scivisData.lights[i];
    if (!l->isVisible)
      continue;

    // Evaluate light contribution
    const Light_EvalRes le = l->eval(l, dg, ray.dir, ray.t0, ray.t, ray.time);
    color = color + le.radiance;
  }
  */
  return color;
}

OSPRAY_END_ISPC_NAMESPACE