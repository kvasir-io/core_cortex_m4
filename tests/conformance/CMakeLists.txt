cmake_minimum_required(VERSION 3.28)

# The core's conformance tree (Kvasir_SDK tests/conformance/conformance.cmake): this package's headers compiled for the
# target with a firmware's flags, paired with a real chip of this core whose `core` is this checkout. Building is the
# test - the contract rows (conformance.cpp), the dependent register writes (instantiate.cpp) and every public header
# alone. The core SVD converts with the write-only guard on the way.
#
# cmake -S tests/conformance -B tests/conformance/build -G Ninja [-DCOMPILE_TARGET=arm_gcc] && cmake --build
# tests/conformance/build
#
# The paired chip is the STM32F413: it converts core_prio4.svd (4 priority bits), core.svd (3 bits) is the SAM E5x's. No
# kvasir_devices: the STM32 package does not include it. Kvasir_SDK and the paired chip: the checkouts next to this one,
# else fetched (FETCHCONTENT_SOURCE_DIR_KVASIR_SDK / _<CHIP> point a lone checkout at local copies).

# Kvasir_SDK: KVASIR_ROOT, else the checkout next to this repository, else fetched; its submodules and the other
# packages: kvasir_locate (Kvasir_SDK cmake/kvasir_locate.cmake)
if(NOT KVASIR_ROOT AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../Kvasir_SDK/cmake/kvasir_locate.cmake")
    get_filename_component(KVASIR_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../Kvasir_SDK" ABSOLUTE)
elseif(NOT KVASIR_ROOT)
    include(FetchContent)
    FetchContent_Declare(
        kvasir_sdk
        GIT_REPOSITORY https://github.com/kvasir-io/Kvasir_SDK.git
        GIT_TAG master
        GIT_SHALLOW TRUE
        GIT_SUBMODULES "" SOURCE_SUBDIR fetch-only)
    FetchContent_MakeAvailable(kvasir_sdk)
    set(KVASIR_ROOT "${kvasir_sdk_SOURCE_DIR}")
endif()
include("${KVASIR_ROOT}/cmake/kvasir_locate.cmake")
include(${KVASIR_ROOT}/tests/conformance/conformance.cmake)
kvasir_core_conformance(PAIRED_CHIP chip_stm32f413)
kvasir_conformance_begin()

project(core_cortex_m4_conformance CXX C ASM)

include(${KVASIR_ROOT}/cmake/util.cmake)

kvasir_conformance_project(
    NAME
    core_cortex_m4_conformance
    SOURCES
    conformance.cpp
    INSTANTIATE
    instantiate.cpp
    CONFIG_HEADER
    kvasir_config.hpp
    HEADERS
    ${CORE_ROOT}/src/core/*.hpp
    ${CORE_ROOT}/src/cortex_common/*.hpp
    NOT_ALONE
    not_alone.txt)
