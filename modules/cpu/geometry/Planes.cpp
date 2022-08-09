// Copyright 2019 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

// ospray
#include "Planes.h"
#include "common/Data.h"
#ifndef OSPRAY_TARGET_DPCPP
// ispc-generated files
#include "geometry/Planes_ispc.h"
#else
#include "geometry/Planes.ih"
#endif

namespace ospray {

Planes::Planes(api::ISPCDevice &device)
    : AddStructShared(device.getIspcrtDevice(), device)
{
#ifndef OSPRAY_TARGET_DPCPP
  getSh()->super.postIntersect =
      reinterpret_cast<ispc::Geometry_postIntersectFct>(
          ispc::Planes_postIntersect_addr());
#endif
}

std::string Planes::toString() const
{
  return "ospray::Planes";
}

void Planes::commit()
{
  coeffsData = getParamDataT<vec4f>("plane.coefficients", true);
  boundsData = getParamDataT<box3f>("plane.bounds");

  createEmbreeUserGeometry((RTCBoundsFunction)&ispc::Planes_bounds);
  getSh()->coeffs = *ispc(coeffsData);
  getSh()->bounds = *ispc(boundsData);
  getSh()->super.numPrimitives = numPrimitives();

  postCreationInfo();
}

size_t Planes::numPrimitives() const
{
  return coeffsData ? coeffsData->size() : 0;
}

} // namespace ospray