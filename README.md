# core_cortex_m4

The Cortex-M4 (Armv7E-M) core layer: `cmake/core.cmake`, `src/core/*.hpp`, and core_cortex_common as the
submodule `src/cortex_common` (its `Debug.hpp` and `CleanCore.hpp` serve every core, included as
`cortex_common/X.hpp`).

- **Priority:** the vendor's choice: `core.svd` has 3 bits (SAM D5x/E5x), `core_prio4.svd` 4 (STM32F4, made by
  `scripts/make_prio4_svd.py`); a chip with 4 sets `CORE_NVIC_PRIO_BITS` before including `core.cmake`.
- **Exceptions:** `CoreInterrupts` adds MemManage, BusFault, UsageFault; `Core::InterruptRules` = Armv7-M (7-10, 13
  reserved, 12 DebugMonitor not served; DDI0403E Table B1-4).
- **Features** (`core/Features.hpp`): `KVASIR_CORE_ARMV7EM` 1, DWT cycle counter 1 (DDI0403E C1.8.8), fault status
  registers 1; no stack limit registers; DHCSR from software 0: Armv7-M makes it IMPLEMENTATION DEFINED (C1.6.2) and
  the ST Cortex-M4 programming manual (PM0214) does not document it.
- **`Core::haltingDebug()`** (`cortex_common/Debug.hpp`): `unknown`, for that reason; `Debug::isDebuggerConnected()`
  stays for a chip that documents the read.
- **`Startup::Core::SecondaryCoreTraits::cpacrEnable`:** CP10/CP11 full access (CPACR bits [21:20], [23:22], DDI0403E
  B3.2.20); no multicore Cortex-M4 chip uses it yet.
- Conformance tree: `tests/conformance` (paired with chip_stm32f413).
