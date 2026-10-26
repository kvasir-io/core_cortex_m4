# How many priority bits the chip implements is the vendor's choice: core.svd has 3 (SAM D5x/E5x), core_prio4.svd 4
# (STM32F4; made from core.svd by scripts/make_prio4_svd.py). A chip with 4 sets CORE_NVIC_PRIO_BITS before the include.
if(NOT DEFINED CORE_NVIC_PRIO_BITS OR CORE_NVIC_PRIO_BITS EQUAL 3)
    set(core_svd_file ${CMAKE_CURRENT_LIST_DIR}/../core.svd)
elseif(CORE_NVIC_PRIO_BITS EQUAL 4)
    set(core_svd_file ${CMAKE_CURRENT_LIST_DIR}/../core_prio4.svd)
else()
    message(FATAL_ERROR "core_cortex_m4: CORE_NVIC_PRIO_BITS ${CORE_NVIC_PRIO_BITS} - there is a core.svd for 3 and 4")
endif()

# Kvasir's write semantics (svd_convert's defaults): a new unclassified write-only field stops the build
kvasir_chip_svd(core_peripherals ${core_svd_file})

# the core's part of the contract (Kvasir_SDK cmake/kvasir_chip.cmake): the TARGET_* variables the toolchain files read,
# and kvasir::core (these headers, the registers above)
kvasir_core(
    CPU
    cortex-m4
    ARCH
    v7e-m
    FPU
    fpv4-sp-d16
    FLOAT_ABI
    hard
    TRIPLE
    thumbv7em-none-eabihf
    INSTRUCTION_MODE
    thumb
    ENDIAN
    little-endian
    INCLUDE_DIR
    ${CMAKE_CURRENT_LIST_DIR}/../src
    REGISTERS
    core_peripherals)
