#pragma once
#include "core_peripherals/FP.hpp"
#include "core_peripherals/SCB.hpp"
#include "core_peripherals/SYSTICK.hpp"
#include "kvasir/Register/Register.hpp"
#include "kvasir/StartUp/LinkerSymbols.hpp"

#include <cstdint>

namespace Kvasir::Startup::Core {

// The FPU comes out of reset with access denied (CPACR = 0), and this core is built for the
// hard-float ABI, so the first frame that spills a d-register faults with NOCP unless cp10 and
// cp11 are opened first. There is no MSPLIM to arm here: the stack limit registers are Armv8-M.
static void startup() {
    using CPACR = Kvasir::Peripheral::SCB::Registers<>::CPACR;
    apply(CPACR::overrideDefaults(write(CPACR::CP<10>::CPValC::full_access),
                                  write(CPACR::CP<11>::CPValC::full_access)));
    asm volatile("dsb\n isb" ::: "memory");
}

// What a warm reset would have done and the SDK's naked reset entry does not (kvasir/StartUp/StartUp.hpp,
// KVASIR_RESET_ENTRY_ASM), for an image not entered from a reset: SysTick stopped, PendSV/SysTick no longer pending,
// VTOR on this image's table, FAULTMASK clear, the fault status of an image before cleared, no lazy FP context
// pending. Before .data and .bss: the register layer only, no static.
[[gnu::always_inline]] inline void cleanCore() {
    using SCB_R     = Kvasir::Peripheral::SCB::Registers<>;
    using SYSTICK_R = Kvasir::Peripheral::SYSTICK::Registers<>;
    using Kvasir::Register::apply;
    using Kvasir::Register::clear;
    using Kvasir::Register::set;
    using Kvasir::Register::write;
    // SYST_CSR (Armv7-M ARM DDI0403E B3.3.3): ENABLE and TICKINT off; CLKSOURCE kept, its reset value is
    // IMPLEMENTATION DEFINED and SystickClock writes it anyway
    apply(clear(SYSTICK_R::CSR::enable), clear(SYSTICK_R::CSR::tickint));
    // ICSR (B3.2.4): PENDSVCLR and PENDSTCLR written over the register's defaults, one store and no read;
    // the SVD leaves the RO status fields read-write, so a plain set() would read first.
    apply(SCB_R::ICSR::overrideDefaults(set(SCB_R::ICSR::pendsvclr), set(SCB_R::ICSR::pendstclr)));
    // VTOR (B3.2.5)
    apply(
      write(SCB_R::VTOR::FULLREGISTER,
            static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&_LINKER_vectors_start_))));
    // FAULTMASK = 0, a special register; PRIMASK stays set (the entry stub)
    asm volatile("cpsie f" ::: "memory");
    // CFSR, HFSR (B3.2.15, B3.2.16): write one to clear, every bit an image before left
    apply(write(SCB_R::CFSR::FULLREGISTER, 0xFFFF'FFFFU));
    apply(write(SCB_R::HFSR::FULLREGISTER, 0xFFFF'FFFFU));
#if defined(__ARM_FP)
    // FPCCR (B3.2.21): no lazy FP context pending; CONTROL.FPCA is 0 already (the entry stub)
    using FP_R = Kvasir::Peripheral::FP::Registers<>;
    apply(write(FP_R::FPCCR::LSPACTValC::lazy_state_preservation_is_not_active));
#endif
    asm volatile("dsb\n isb" ::: "memory");
}

}   // namespace Kvasir::Startup::Core
