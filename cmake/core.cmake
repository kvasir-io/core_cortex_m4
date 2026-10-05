set(TARGET_FLOAT_ABI hard)
set(TARGET_FPU fpv4-sp-d16)
set(TARGET_CPU cortex-m4)
set(TARGET_ARCH v7e-m)
set(TARGET_TRIPLE thumbv7em-none-eabihf)
set(TARGET_ARM_INSTRUCTION_MODE thumb)
set(TARGET_ENDIAN little-endian)

# How many priority bits the chip implements is the vendor's choice: core.svd has 3 (SAM D5x/E5x), core_prio4.svd 4
# (STM32F4; made from core.svd by scripts/make_prio4_svd.py). A chip with 4 sets CORE_NVIC_PRIO_BITS before the include.
if(NOT DEFINED CORE_NVIC_PRIO_BITS OR CORE_NVIC_PRIO_BITS EQUAL 3)
    set(core_svd_file ${CMAKE_CURRENT_LIST_DIR}/../core.svd)
elseif(CORE_NVIC_PRIO_BITS EQUAL 4)
    set(core_svd_file ${CMAKE_CURRENT_LIST_DIR}/../core_prio4.svd)
else()
    message(FATAL_ERROR "core_cortex_m4: CORE_NVIC_PRIO_BITS ${CORE_NVIC_PRIO_BITS} - there is a core.svd for 3 and 4")
endif()

# the write-only guard: every write-only field of the SVD is classified (oneToSet, a key, or <!-- Kvasir: write-only
# accepted -->), so a new one stops the build; registers with no readable field are never read
svd_convert(
    core_peripherals
    SVD_FILE
    ${core_svd_file}
    OUTPUT_DIRECTORY
    core_peripherals
    WRITE_ONLY_GUARD
    error
    WRITE_ONLY_REGISTERS
    derived
    WRITE_ONLY_MASK
    ON)
