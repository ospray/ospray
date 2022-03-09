// Copyright 2009-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "PerspectiveCamera.h"
#include "camera/PerspectiveCamera_ispc.h"

namespace ospray {

PerspectiveCamera::PerspectiveCamera()
{
  ispcEquivalent = ispc::PerspectiveCamera_create(this);
}

std::string PerspectiveCamera::toString() const
{
  return "ospray::PerspectiveCamera";
}

void PerspectiveCamera::commit()
{
  Camera::commit();

  fovy = getParam<float>("fovy", 60.f);
  aspect = getParam<float>("aspect", 1.f);
  apertureRadius = getParam<float>("apertureRadius", 0.f);
  focusDistance = getParam<float>("focusDistance", 1.f);
  architectural = getParam<bool>("architectural", false);
  stereoMode = (OSPStereoMode)getParam<uint8_t>(
      "stereoMode", getParam<int32_t>("stereoMode", OSP_STEREO_NONE));
  // the default 63.5mm represents the average human IPD
  interpupillaryDistance = getParam<float>("interpupillaryDistance", 0.0635f);

  switch (stereoMode) {
  case OSP_STEREO_SIDE_BY_SIDE:
    aspect *= 0.5f;
    break;
  case OSP_STEREO_TOP_BOTTOM:
    aspect *= 2.f;
    break;
  default:
    break;
  }

  imgPlaneSize.y = 2.f * tanf(deg2rad(0.5f * fovy));
  imgPlaneSize.x = imgPlaneSize.y * aspect;

  ispc::PerspectiveCamera_set(getIE(),
      (const ispc::vec3f &)pos,
      (const ispc::vec3f &)dir,
      (const ispc::vec3f &)up,
      (const ispc::vec2f &)imgPlaneSize,
      apertureRadius / (imgPlaneSize.x * focusDistance),
      focusDistance,
      aspect,
      architectural,
      stereoMode,
      interpupillaryDistance);
}

ProjectedPoint PerspectiveCamera::projectPoint(const vec3f &p) const
{
  // We find the intersection of the ray through the point with the virtual
  // film plane, then find the vector to this point from the origin of the
  // film plane (screenDir) and project this point onto the x/y axes of
  // the plane.
  const vec3f v = p - pos;
  const vec3f r = normalize(v);
  const float denom = dot(-r, -dir);
  if (denom == 0.0) {
    return ProjectedPoint(
        vec3f(-1, -1, std::numeric_limits<float>::infinity()), -1);
  }
  const float t = 1.0 / denom;

  const vec3f screenDir = r * t - dir_00;
  const vec2f sp = vec2f(dot(screenDir, normalize(dir_du)),
                       dot(screenDir, normalize(dir_dv)))
      / imgPlaneSize;
  const float depth = sign(t) * length(v);
  // TODO: Depth of field radius, stereo
  return ProjectedPoint(vec3f(sp.x, sp.y, depth), 0);
}

} // namespace ospray
