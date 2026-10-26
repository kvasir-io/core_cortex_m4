#pragma once

// The common exceptions come from core_cortex_common; these are the configurable fault handlers
// an Armv7-M core adds to them. There is no SecureFault (-9): that one is Armv8-M.
#include "cortex_common/CoreInterrupts.hpp"
#include "cortex_common/InterruptRules.hpp"
#include "kvasir/Mpl/Types.hpp"

namespace Kvasir {
struct CoreInterrupts : CommonCoreInterrupts {
    static constexpr Type<-12> memoryManagement{};
    static constexpr Type<-11> busFault{};
    static constexpr Type<-10> usageFault{};
};

namespace Core {
    // Armv7-M's exception rules. The three configurable faults are enabled in SHCSR by
    // Fault::Core::EarlyInitList rather than through the NVIC, so none of them needs an enable.
    struct InterruptRules : CoreInterruptRules<ArmProfile::v7m> {
        using FaultInterruptIndexs = brigand::list<decltype(CoreInterrupts::hardFault),
                                                   decltype(CoreInterrupts::memoryManagement),
                                                   decltype(CoreInterrupts::busFault),
                                                   decltype(CoreInterrupts::usageFault)>;
    };
}   // namespace Core
}   // namespace Kvasir
