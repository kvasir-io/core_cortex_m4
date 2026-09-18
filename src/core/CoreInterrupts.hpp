#pragma once

// The common exceptions come from core_cortex_common; these are the configurable fault handlers
// an Armv7-M core adds to them. There is no SecureFault (-9): that one is Armv8-M.
#include "cortex_common/CoreInterrupts.hpp"

namespace Kvasir {
struct CoreInterrupts : CommonCoreInterrupts {
    static constexpr Type<-12> memoryManagement{};
    static constexpr Type<-11> busFault{};
    static constexpr Type<-10> usageFault{};
};
}   // namespace Kvasir
