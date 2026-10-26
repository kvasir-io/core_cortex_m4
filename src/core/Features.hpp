#pragma once
// What this core package provides, checked against the SDK's derivation (kvasir/Port/Capabilities.hpp, the macros
// `#if` tests above the core use): a core whose facts the compiler macros do not give stops the build here.
#include "kvasir/Port/Capabilities.hpp"

static_assert(KVASIR_CORE_ARMV6M == 0);
static_assert(KVASIR_CORE_ARMV7EM == 1);
static_assert(KVASIR_CORE_ARMV8M_MAIN == 0);
static_assert(KVASIR_CORE_HAS_CYCLE_COUNTER == 1);   // DWT_CYCCNT, Armv7-M ARM DDI0403E C1.8.8
// "Access to the DHCSR from software running on the processor is IMPLEMENTATION DEFINED" (DDI0403E C1.6.2), and
// the ST Cortex-M4 programming manual PM0214 says nothing about it: not counted on.
static_assert(KVASIR_CORE_HAS_DEBUGEN_READ == 0);
static_assert(KVASIR_CORE_HAS_STACK_LIMITS == 0);   // no MSPLIM/PSPLIM before Armv8-M
static_assert(KVASIR_CORE_HAS_FAULT_STATUS == 1);   // CFSR/HFSR/MMFAR/BFAR
