// ======================================================================== //
// Copyright 2009-2019 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

// ospray
#include "VolumetricModel.h"
#include "transferFunction/TransferFunction.h"
// openvkl
#include "openvkl/openvkl.h"
// ispc exports
#include "Volume_ispc.h"
#include "VolumetricModel_ispc.h"

namespace ospray {

  VolumetricModel::VolumetricModel(Volume *_volume)
  {
    managedObjectType = OSP_VOLUMETRIC_MODEL;

    if (_volume == nullptr)
      throw std::runtime_error("volumetric model received null volume");

    volume = _volume;

    this->ispcEquivalent = ispc::VolumetricModel_create(this, volume->getIE());
  }

  std::string VolumetricModel::toString() const
  {
    return "ospray::VolumetricModel";
  }

  void VolumetricModel::commit()
  {
    auto *transferFunction =
        (TransferFunction *)getParamObject("transferFunction", nullptr);

    if (transferFunction == nullptr)
      throw std::runtime_error("volumetric model must have 'transferFunction'");

    // create value selector using transfer function and pass to volume
    if (volume->vklVolume)
    {
      if (vklValueSelector) {
        vklRelease(vklValueSelector);
        vklValueSelector = nullptr;
      }

      vklValueSelector = vklNewValueSelector(volume->vklVolume);
      std::vector<range1f> valueRanges = transferFunction->getPositiveOpacityValueRanges();
      vklValueSelectorSetRanges(vklValueSelector,
                                valueRanges.size(),
                                (const vkl_range1f *)valueRanges.data());
      vklCommit(vklValueSelector);
      ispc::VolumetricModel_set_valueSelector(ispcEquivalent, vklValueSelector);
    }

    // Finish getting/setting other appearance information //
    volumeBounds = volume->bounds;

    ispc::VolumetricModel_set(ispcEquivalent,
                              getParam<float>("samplingRate", 0.125f),
                              transferFunction->getIE(),
                              (const ispc::box3f &)volumeBounds,
                              getParam<float>("densityScale", 1.f),
                              getParam<float>("anisotropy", 0.f));
  }

  RTCGeometry VolumetricModel::embreeGeometryHandle() const
  {
    return volume->embreeGeometry;
  }

  box3f VolumetricModel::bounds() const
  {
    return volumeBounds;
  }

  Ref<Volume> VolumetricModel::getVolume() const
  {
    return volume;
  }

  void VolumetricModel::setGeomID(int geomID)
  {
    ispc::Volume_set_geomID(volume->getIE(), geomID);
  }

  OSPTYPEFOR_DEFINITION(VolumetricModel *);

}  // namespace ospray
