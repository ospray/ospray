## Copyright 2009 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

# to run:  ./run_tests.ps1 <path to ospray source> <reference images ISA> [TEST_MPI]
# a new folder is created called build_regression_tests with results

$osprayDir=$args[0]

$testMPI = $FALSE
$testMultiDevice = $FALSE
foreach ($arg in $args) {
  if ( $arg -eq "TEST_MPI" ) {
    $testMPI = $TRUE
  }
  if ( $arg -eq "TEST_MULTIDEVICE" ) {
    $testMultiDevice = $TRUE
  }
}

md build_regression_tests
cd build_regression_tests

$exitCode = 0

cmake -D OSPRAY_TEST_ISA=AVX512SKX $osprayDir/test_image_data
if ($LastExitCode) { $exitCode++ }
cmake --build . --config Release --target ospray_test_data
if ($LastExitCode) { $exitCode++ }

### Excluded tests on GPU
#########################
# Clipping unsupported
$test_filters="ClippingParallel.planes"
$test_filters+=":TestScenesClipping/FromOsprayTesting.*"
$test_filters+=":TestScenesMaxDepth/FromOsprayTestingMaxDepth.test_scenes/1"
$test_filters+=":TestScenesMaxDepth/FromOsprayTestingMaxDepth.test_scenes/2"
$test_filters+=":TestMotionBlur/MotionBlurBoxes.*"
$test_filters+=":CameraRollingShutter/MotionBlurBoxes.*"
$test_filters+=":CameraStereoRollingShutter/MotionBlurBoxes.*"
# Subdivision surfaces unsupported
$test_filters+=":TestScenesGeometry/FromOsprayTesting.test_scenes/15"
$test_filters+=":TestScenesGeometry/FromOsprayTesting.test_scenes/16"
$test_filters+=":TestScenesGeometry/FromOsprayTesting.test_scenes/17"
$test_filters+=":Color/Interpolation.Interpolation/4"
$test_filters+=":Color/Interpolation.Interpolation/5"
$test_filters+=":Color/Interpolation.Interpolation/6"
$test_filters+=":Color/Interpolation.Interpolation/7"
$test_filters+=":Texcoord/Interpolation.Interpolation/2"
$test_filters+=":Texcoord/Interpolation.Interpolation/3"
# Multiple volumes unsupported
$test_filters+=":TestScenesVolumes/FromOsprayTesting.test_scenes/3"
$test_filters+=":TestScenesVolumes/FromOsprayTesting.test_scenes/4"
$test_filters+=":TestScenesVolumes/FromOsprayTesting.test_scenes/5"
$test_filters+=":TestScenesVolumesStrictParams/FromOsprayTesting.*"
# Requires non-overlapping multiple volume support on GPU
$test_filters+=":ObjectInstance/IDBuffer.*"
# Instancing test includes multiple volumes
$test_filters+=":TestScenesGeometry/FromOsprayTesting.test_scenes/24"
$test_filters+=":TestScenesGeometry/FromOsprayTesting.test_scenes/25"
$test_filters+=":TestScenesGeometry/FromOsprayTesting.test_scenes/26"
# Variance termination is not quite right
$test_filters+=":TestScenesVariance/FromOsprayTestingVariance.testScenes/0"
# 'mix' material not supported on GPU (not practical to implement without fn ptr)
$test_filters+=":TestScenesPtMaterials/FromOsprayTesting.test_scenes/8"
# Crashing FIXME
$test_filters+=":Primitive/IDBuffer.*"

# Different noise
$test_filters+=":TestScenesVolumes/FromOsprayTesting.test_scenes/1"

# Artifacts
$test_filters+=":TestScenesPtMaterials/FromOsprayTesting.test_scenes/5"

## Windows only (driver?)

# Artifacts
$test_filters+=":TestScenesPtMaterials/FromOsprayTesting.test_scenes/4"

# Artifacts with dpcpp (icx is fine)
$test_filters+=":TestScenesGeometry/FromOsprayTesting.test_scenes/23"
$test_filters+=":TestScenesVolumes/FromOsprayTesting.test_scenes/7"

# Artifacts with icx (dpcpp is fine)
$test_filters+=":Light/GeometricLight.parameter/2"
$test_filters+=":LightTexture/GeometricLight.parameter/0"
$test_filters+=":LightTexture/GeometricLight.parameter/1"


$env:ONEAPI_DEVICE_SELECTOR="level_zero:*"
$env:SYCL_CACHE_PERSISTENT="1"
$env:OIDN_VERBOSE="2"

md failed-gpu
ospTestSuite.exe --gtest_output=xml:tests.xml --baseline-dir=regression_test_baseline\ --failed-dir=failed-gpu --osp:load-modules=gpu --osp:device=gpu --gtest_filter="-$test_filters" --own-SYCL
if ($LastExitCode) { $exitCode++ }

$env:OSPRAY_ALLOW_DEVICE_MEMORY = "1"
ospTestSuite.exe --gtest_output=xml:tests.xml --baseline-dir=regression_test_baseline\ --failed-dir=failed-gpu --osp:load-modules=gpu --osp:device=gpu --gtest_filter=SharedData/TestUSMSharing.structured_regular/2 --own-SYCL
if ($LastExitCode) { $exitCode++ }
$env:OSPRAY_ALLOW_DEVICE_MEMORY = "0"

if ( $testMultiDevice ) {
  md failed-multidevice
  $env:OSPRAY_NUM_SUBDEVICES = "2"
  ospTestSuite.exe --osp:load-modules=multidevice_cpu --osp:device=multidevice --gtest_output=xml:tests-multidevice.xml --baseline-dir=regression_test_baseline\ --failed-dir=failed-multidevice --gtest_filter="-$test_filters"
  if ($LastExitCode) { $exitCode++ }
}

if ( $testMPI ) {
  md failed-mpi-gpu
  $env:OSPRAY_MPI_DISTRIBUTED_GPU = "1"
  mpiexec.exe -np 1 ospTestSuite.exe --osp:load-modules=mpi_offload --osp:device=mpiOffload --gtest_output=xml:tests-mpi.xml --baseline-dir=regression_test_baseline\ --failed-dir=failed-mpi-gpu --gtest_filter="-$test_filters" : -np 2 ospray_mpi_worker.exe
  if ($LastExitCode) { $exitCode++ }

  md failed-mpi-gpu-data-parallel
  $test_filters="MPIDistribTestScenesVolumes/MPIFromOsprayTesting.test_scenes/1" # FIXME
  mpiexec.exe -np 3 -prepend-rank ospMPIDistribTestSuite.exe --gtest_output=xml:tests-mpi.xml --baseline-dir=regression_test_baseline\ --failed-dir=failed-mpi-gpu-data-parallel --gtest_filter="-$test_filters"
  if ($LastExitCode) { $exitCode++ }
}

exit $exitCode
