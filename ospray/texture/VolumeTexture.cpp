// ======================================================================== //
// Copyright 2009-2018 Intel Corporation                                    //
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

#include "VolumeTexture.h"
#include "VolumeTexture_ispc.h"

#include "../common/Data.h"

namespace ospray {

  std::string VolumeTexture::toString() const
  {
    return "ospray::VolumeTexture";
  }

  void VolumeTexture::commit()
  {
    if (this->ispcEquivalent)
      ispc::delete_uniform(ispcEquivalent);

    auto v = dynamic_cast<Volume*>(getParamObject("volume"));

    if (v == nullptr)
      throw std::runtime_error("no volume specified on 'volume' texture!");

    volume = v;

    this->ispcEquivalent = ispc::VolumeTexture_create(v);
  }

  OSP_REGISTER_TEXTURE(VolumeTexture, volume);

} // ::ospray