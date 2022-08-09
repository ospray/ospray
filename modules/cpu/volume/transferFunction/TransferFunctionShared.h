// Copyright 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifdef __cplusplus
#include "common/StructShared.h"
namespace ispc {
#endif // __cplusplus

#if defined(__cplusplus) && !defined(OSPRAY_TARGET_DPCPP)
typedef void *TransferFunction_getFct;
typedef void *TransferFunction_getMaxOpacityFct;
#else
struct TransferFunction;

typedef vec4f (*TransferFunction_getFct)(
    const TransferFunction *uniform self, float value);
typedef float (*TransferFunction_getMaxOpacityFct)(
    const TransferFunction *uniform self, const range1f &valueRange);
#endif

struct TransferFunction
{
  range1f valueRange;

  TransferFunction_getFct get;
  TransferFunction_getMaxOpacityFct getMaxOpacity;

#ifdef __cplusplus
  TransferFunction()
      : valueRange(0.f, 1.f), get(nullptr), getMaxOpacity(nullptr)
  {}
};
} // namespace ispc
#else
};
#endif // __cplusplus