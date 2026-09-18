set(TARGET_FLOAT_ABI hard)
set(TARGET_FPU fpv4-sp-d16)
set(TARGET_CPU cortex-m4)
set(TARGET_ARCH v7e-m)
set(TARGET_TRIPLE thumbv7em-none-eabihf)
set(TARGET_ARM_INSTRUCTION_MODE thumb)
set(TARGET_ENDIAN little-endian)

svd_convert(core_peripherals SVD_FILE ${CMAKE_CURRENT_LIST_DIR}/../core.svd OUTPUT_DIRECTORY core_peripherals)
