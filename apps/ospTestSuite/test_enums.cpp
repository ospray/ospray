// Copyright 2019-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include "embree3/rtcore.h"
#include "openvkl/openvkl.h"
#include "ospray/OSPEnums.h"

TEST(Enums, VKLLogLevel)
{
  ASSERT_LE(sizeof(OSPLogLevel), sizeof(VKLLogLevel));
  ASSERT_EQ(OSP_LOG_DEBUG, VKL_LOG_DEBUG);
  ASSERT_EQ(OSP_LOG_INFO, VKL_LOG_INFO);
  ASSERT_EQ(OSP_LOG_WARNING, VKL_LOG_WARNING);
  ASSERT_EQ(OSP_LOG_ERROR, VKL_LOG_ERROR);
  ASSERT_EQ(OSP_LOG_NONE, VKL_LOG_NONE);
}

TEST(Enums, VKLDataType)
{
  ASSERT_LE(sizeof(OSPDataType), sizeof(VKLDataType));
  ASSERT_EQ(OSP_BOOL, VKL_BOOL);
  ASSERT_EQ(OSP_CHAR, VKL_CHAR);
  ASSERT_EQ(OSP_VEC2C, VKL_VEC2C);
  ASSERT_EQ(OSP_VEC3C, VKL_VEC3C);
  ASSERT_EQ(OSP_VEC4C, VKL_VEC4C);
  ASSERT_EQ(OSP_UCHAR, VKL_UCHAR);
  ASSERT_EQ(OSP_VEC2UC, VKL_VEC2UC);
  ASSERT_EQ(OSP_VEC3UC, VKL_VEC3UC);
  ASSERT_EQ(OSP_VEC4UC, VKL_VEC4UC);
  ASSERT_EQ(OSP_SHORT, VKL_SHORT);
  ASSERT_EQ(OSP_VEC2S, VKL_VEC2S);
  ASSERT_EQ(OSP_VEC3S, VKL_VEC3S);
  ASSERT_EQ(OSP_VEC4S, VKL_VEC4S);
  ASSERT_EQ(OSP_USHORT, VKL_USHORT);
  ASSERT_EQ(OSP_VEC2US, VKL_VEC2US);
  ASSERT_EQ(OSP_VEC3US, VKL_VEC3US);
  ASSERT_EQ(OSP_VEC4US, VKL_VEC4US);
  ASSERT_EQ(OSP_INT, VKL_INT);
  ASSERT_EQ(OSP_VEC2I, VKL_VEC2I);
  ASSERT_EQ(OSP_VEC3I, VKL_VEC3I);
  ASSERT_EQ(OSP_VEC4I, VKL_VEC4I);
  ASSERT_EQ(OSP_UINT, VKL_UINT);
  ASSERT_EQ(OSP_VEC2UI, VKL_VEC2UI);
  ASSERT_EQ(OSP_VEC3UI, VKL_VEC3UI);
  ASSERT_EQ(OSP_VEC4UI, VKL_VEC4UI);
  ASSERT_EQ(OSP_LONG, VKL_LONG);
  ASSERT_EQ(OSP_VEC2L, VKL_VEC2L);
  ASSERT_EQ(OSP_VEC3L, VKL_VEC3L);
  ASSERT_EQ(OSP_VEC4L, VKL_VEC4L);
  ASSERT_EQ(OSP_ULONG, VKL_ULONG);
  ASSERT_EQ(OSP_VEC2UL, VKL_VEC2UL);
  ASSERT_EQ(OSP_VEC3UL, VKL_VEC3UL);
  ASSERT_EQ(OSP_VEC4UL, VKL_VEC4UL);
  ASSERT_EQ(OSP_HALF, VKL_HALF);
  ASSERT_EQ(OSP_VEC2H, VKL_VEC2H);
  ASSERT_EQ(OSP_VEC3H, VKL_VEC3H);
  ASSERT_EQ(OSP_VEC4H, VKL_VEC4H);
  ASSERT_EQ(OSP_FLOAT, VKL_FLOAT);
  ASSERT_EQ(OSP_VEC2F, VKL_VEC2F);
  ASSERT_EQ(OSP_VEC3F, VKL_VEC3F);
  ASSERT_EQ(OSP_VEC4F, VKL_VEC4F);
  ASSERT_EQ(OSP_DOUBLE, VKL_DOUBLE);
  ASSERT_EQ(OSP_VEC2D, VKL_VEC2D);
  ASSERT_EQ(OSP_VEC3D, VKL_VEC3D);
  ASSERT_EQ(OSP_VEC4D, VKL_VEC4D);
  ASSERT_EQ(OSP_BOX1I, VKL_BOX1I);
  ASSERT_EQ(OSP_BOX2I, VKL_BOX2I);
  ASSERT_EQ(OSP_BOX3I, VKL_BOX3I);
  ASSERT_EQ(OSP_BOX4I, VKL_BOX4I);
  ASSERT_EQ(OSP_BOX1F, VKL_BOX1F);
  ASSERT_EQ(OSP_BOX2F, VKL_BOX2F);
  ASSERT_EQ(OSP_BOX3F, VKL_BOX3F);
  ASSERT_EQ(OSP_BOX4F, VKL_BOX4F);
  ASSERT_EQ(OSP_DATA, VKL_DATA);
  ASSERT_EQ(OSP_LINEAR2F, VKL_LINEAR2F);
  ASSERT_EQ(OSP_LINEAR3F, VKL_LINEAR3F);
  ASSERT_EQ(OSP_AFFINE2F, VKL_AFFINE2F);
  ASSERT_EQ(OSP_AFFINE3F, VKL_AFFINE3F);
  ASSERT_EQ(OSP_RAW, VKL_RAW);
  ASSERT_EQ(OSP_BYTE, VKL_BYTE);
  ASSERT_EQ(OSP_VOID_PTR, VKL_VOID_PTR);
  ASSERT_EQ(OSP_STRING, VKL_STRING);
  ASSERT_EQ(OSP_OBJECT, VKL_OBJECT);
  // those are different object types:
  // ASSERT_EQ(OSP_VOLUME, VKL_VOLUME);
}

TEST(Enums, VKLUnstructuredCellType)
{
  ASSERT_LE(sizeof(OSPUnstructuredCellType), sizeof(VKLUnstructuredCellType));
  ASSERT_EQ(OSP_TETRAHEDRON, VKL_TETRAHEDRON);
  ASSERT_EQ(OSP_HEXAHEDRON, VKL_HEXAHEDRON);
  ASSERT_EQ(OSP_WEDGE, VKL_WEDGE);
  ASSERT_EQ(OSP_PYRAMID, VKL_PYRAMID);
}

TEST(Enums, VKLAMRMethod)
{
  ASSERT_LE(sizeof(OSPAMRMethod), sizeof(VKLAMRMethod));
  ASSERT_EQ(OSP_AMR_CURRENT, VKL_AMR_CURRENT);
  ASSERT_EQ(OSP_AMR_FINEST, VKL_AMR_FINEST);
  ASSERT_EQ(OSP_AMR_OCTANT, VKL_AMR_OCTANT);
}

TEST(Enums, VKLFilter)
{
  ASSERT_LE(sizeof(OSPVolumeFilter), sizeof(VKLFilter));
  ASSERT_EQ(OSP_VOLUME_FILTER_NEAREST, VKL_FILTER_NEAREST);
  ASSERT_EQ(OSP_VOLUME_FILTER_TRILINEAR, VKL_FILTER_TRILINEAR);
  ASSERT_EQ(OSP_VOLUME_FILTER_TRICUBIC, VKL_FILTER_TRICUBIC);
}
 
TEST(Enums, VKLError)
{
  ASSERT_LE(sizeof(OSPError), sizeof(VKLError));
  ASSERT_EQ(OSP_NO_ERROR, VKL_NO_ERROR);
  ASSERT_EQ(OSP_UNKNOWN_ERROR, VKL_UNKNOWN_ERROR);
  ASSERT_EQ(OSP_INVALID_ARGUMENT, VKL_INVALID_ARGUMENT);
  ASSERT_EQ(OSP_INVALID_OPERATION, VKL_INVALID_OPERATION);
  ASSERT_EQ(OSP_OUT_OF_MEMORY, VKL_OUT_OF_MEMORY);
  ASSERT_EQ(OSP_UNSUPPORTED_CPU, VKL_UNSUPPORTED_CPU);
}

TEST(Enums, RTCSubdivisionMode)
{
  ASSERT_LE(sizeof(OSPSubdivisionMode), sizeof(RTCSubdivisionMode));
  ASSERT_EQ(OSP_SUBDIVISION_NO_BOUNDARY, RTC_SUBDIVISION_MODE_NO_BOUNDARY);
  ASSERT_EQ(
      OSP_SUBDIVISION_SMOOTH_BOUNDARY, RTC_SUBDIVISION_MODE_SMOOTH_BOUNDARY);
  ASSERT_EQ(OSP_SUBDIVISION_PIN_CORNERS, RTC_SUBDIVISION_MODE_PIN_CORNERS);
  ASSERT_EQ(OSP_SUBDIVISION_PIN_BOUNDARY, RTC_SUBDIVISION_MODE_PIN_BOUNDARY);
  ASSERT_EQ(OSP_SUBDIVISION_PIN_ALL, RTC_SUBDIVISION_MODE_PIN_ALL);
}

TEST(Enums, RTCError)
{
  ASSERT_LE(sizeof(OSPError), sizeof(RTCError));
  ASSERT_EQ(OSP_NO_ERROR, RTC_ERROR_NONE);
  ASSERT_EQ(OSP_UNKNOWN_ERROR, RTC_ERROR_UNKNOWN);
  ASSERT_EQ(OSP_INVALID_ARGUMENT, RTC_ERROR_INVALID_ARGUMENT);
  ASSERT_EQ(OSP_INVALID_OPERATION, RTC_ERROR_INVALID_OPERATION);
  ASSERT_EQ(OSP_OUT_OF_MEMORY, RTC_ERROR_OUT_OF_MEMORY);
  ASSERT_EQ(OSP_UNSUPPORTED_CPU, RTC_ERROR_UNSUPPORTED_CPU);
}
