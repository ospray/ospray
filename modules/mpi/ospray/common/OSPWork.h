// Copyright 2009 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

#include <ospray/ospray.h>
#include "MPICommon.h"
#include "common/ObjectHandle.h"

#include "rkcommon/memory/RefCount.h"
#include "rkcommon/networking/DataStreaming.h"
#include "rkcommon/networking/Fabric.h"
#include "rkcommon/platform.h"
#include "rkcommon/utility/ArrayView.h"
#include "rkcommon/utility/FixedArray.h"

namespace ospray {
namespace mpi {
namespace work {

using namespace rkcommon;

enum TAG
{
  NONE,
  NEW_RENDERER,
  NEW_WORLD,
  NEW_GEOMETRY,
  NEW_GEOMETRIC_MODEL,
  NEW_VOLUME,
  NEW_VOLUMETRIC_MODEL,
  NEW_CAMERA,
  NEW_TRANSFER_FUNCTION,
  NEW_IMAGE_OPERATION,
  NEW_MATERIAL,
  NEW_LIGHT,
  DATA_TRANSFER,
  NEW_SHARED_DATA,
  NEW_DATA,
  COPY_DATA,
  NEW_TEXTURE,
  NEW_GROUP,
  NEW_INSTANCE,
  COMMIT,
  RELEASE,
  RETAIN,
  LOAD_MODULE,
  CREATE_FRAMEBUFFER,
  MAP_FRAMEBUFFER,
  GET_VARIANCE,
  RESET_ACCUMULATION,
  RENDER_FRAME,
  SET_PARAM,
  REMOVE_PARAM,
  PICK,
  GET_BOUNDS,
  FUTURE_IS_READY,
  FUTURE_WAIT,
  FUTURE_CANCEL,
  FUTURE_GET_PROGRESS,
  FUTURE_GET_TASK_DURATION,
  FINALIZE
};

struct FrameBufferInfo : rkcommon::memory::RefCountedObject
{
  vec2i size = vec2i(0, 0);
  OSPFrameBufferFormat format = OSP_FB_NONE;
  uint32_t channels = 0;

  FrameBufferInfo() = default;
  FrameBufferInfo(
      const vec2i &size, OSPFrameBufferFormat format, uint32_t channels);

  size_t pixelSize(uint32_t channel) const;
  size_t getNumPixels() const;
};

struct OSPState
{
  std::unordered_map<int64_t, OSPObject> objects;
  std::unordered_map<int64_t, Ref<FrameBufferInfo>> framebuffers;

  // Large data which is transferred separately from the command buffer,
  // prior to sending the command buffer
  std::queue<std::shared_ptr<utility::FixedArray<uint8_t>>> dataTransfers;

  template <typename T>
  T getObject(int64_t handle)
  {
    return reinterpret_cast<T>(objects[handle]);
  }
};

void dispatchWork(TAG t,
    OSPState &state,
    networking::BufferReader &cmdBuf,
    networking::Fabric &fabric);

const char *tagName(work::TAG t);
} // namespace work
} // namespace mpi
} // namespace ospray
