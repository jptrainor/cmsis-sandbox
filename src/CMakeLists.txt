# SPDX-FileCopyrightText: 2024 Jim Trainor <https://github.com/jptrainor/cmsis-sandbox/issues>
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# error if pico sdk path not set
message("PICO_SDK_PATH:" $ENV{PICO_SDK_PATH})
if(DEFINED ENV{PICO_SDK_PATH})
    message(STATUS "PICO_SDK_PATH environment variable defined")
endif()

set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# Pull in CMSIS-DSP
set(CMSISCORE "$ENV{PICO_SDK_PATH}/src/rp2_common/cmsis/stub/CMSIS/Core")
set(DISABLEFLOAT16 ON)
include(FetchContent)
FetchContent_Declare(cmsisdsp
   GIT_REPOSITORY https://github.com/ARM-software/CMSIS-DSP.git
   GIT_TAG "v1.15.0"
)
FetchContent_MakeAvailable(cmsisdsp)

# Declare the pico project
project(cmsis-sandbox C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
set(PICO_CXX_ENABLE_EXCEPTIONS 1)
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(cmsis-sandbox
  cmsis-sandbox.cpp
  dsp/DspMain.cpp
  dsp/MemDebug.cpp
  dsp/CmsisTypeFactory.cpp
  dsp/CmsisFft.cpp
  dsp/CmsisDecimate.cpp
  dsp/Signal.cpp
  dsp/DecimateFIR.cpp
  dsp/FirSource.cpp
  dsp/FftTest.cpp
  dsp/FftTestRunner.cpp
  dsp/WindowFunction.cpp
  dsp/DecimateTest.cpp
  dsp/DecimateTestRunner.cpp
  dsp/Report.cpp
  platform/PicoPlatform.cpp )

pico_set_program_name(cmsis-sandbox "cmsis-sandbox")
pico_set_program_version(cmsis-sandbox "0.1")

pico_enable_stdio_uart(cmsis-sandbox 1)
pico_enable_stdio_usb(cmsis-sandbox 0)

# Dependency to ensure that libCMSISDSP.a is built
add_dependencies(cmsis-sandbox CMSISDSP)

# Add pico and cmsis libraries to the build
target_link_libraries(cmsis-sandbox
  pico_stdlib
  ${cmsisdsp_BINARY_DIR}/Source/libCMSISDSP.a
)

# Add pico and cmsis include paths
target_include_directories(cmsis-sandbox PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/dsp
  ${CMAKE_CURRENT_LIST_DIR}/platform
  ${cmsisdsp_SOURCE_DIR}/Include
  ${PICO_SDK_PATH}/src/rp2_common/cmsis/stub/CMSIS/Core/Include
)

# The sandbox platform definition.
target_compile_definitions(cmsis-sandbox PRIVATE SANDBOX_PLATFORM=RP2040)

pico_add_extra_outputs(cmsis-sandbox)


