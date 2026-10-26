cmake_minimum_required(VERSION 3.28)

# The core's host checks: core_prio4.svd (the 4-priority-bit core SVD the STM32F4 chips convert) must be what
# scripts/make_prio4_svd.py makes of core.svd. Needs python3 only. The conformance tree is tests/conformance.
#
# cmake -S tests -B tests/build -G Ninja && ctest --test-dir tests/build
project(core_cortex_m4_tests LANGUAGES NONE)

enable_testing()

find_package(
    Python3
    COMPONENTS Interpreter
    REQUIRED)
add_test(NAME core_prio4_svd_up_to_date COMMAND ${Python3_EXECUTABLE} -B
                                                ${CMAKE_CURRENT_LIST_DIR}/../scripts/make_prio4_svd.py --check)
