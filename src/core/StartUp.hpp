#pragma once
// The core's start: startup() here, cleanCore() and SecondaryCoreTraits from core_cortex_common (one body for every
// core, chosen by the capability macros).
#include "core/Barrier.hpp"
#include "core_peripherals/SCB.hpp"
#include "cortex_common/CleanCore.hpp"
#include "cortex_common/SecondaryCoreTraits.hpp"
#include "kvasir/Register/Register.hpp"

namespace Kvasir::Startup::Core {

// The FPU comes out of reset with access denied (CPACR = 0), and this core is built for the
// hard-float ABI, so the first frame that spills a d-register faults with NOCP unless cp10 and
// cp11 are opened first. There is no MSPLIM to arm here: the stack limit registers are Armv8-M.
inline void startup() {
    using CPACR = Kvasir::Peripheral::SCB::Registers<>::CPACR;
    apply(CPACR::overrideDefaults(write(CPACR::CP<10>::CPValC::full_access),
                                  write(CPACR::CP<11>::CPValC::full_access)));
    Kvasir::Core::dsb();
    Kvasir::Core::isb();
}

}   // namespace Kvasir::Startup::Core
