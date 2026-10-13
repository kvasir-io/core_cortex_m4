#pragma once
#include "core_peripherals/DCB.hpp"
#include "core_peripherals/DWT.hpp"
#include "kvasir/Register/Register.hpp"

#include <cstdint>

namespace Kvasir::Core::Debug {

using DCB_R = Kvasir::Peripheral::DCB::Registers<>;

// Returns true if a debugger has enabled halting debug (DHCSR.C_DEBUGEN).
// This bit is set by the debugger over SWD when it attaches; software can only read it.
[[nodiscard]] static inline bool isDebuggerConnected() {
    using Kvasir::Register::apply;
    using Kvasir::Register::read;
    // This core's SVD gives C_DEBUGEN an enumeration, so the read is a scoped enum.
    return static_cast<unsigned>(get<0>(apply(read(DCB_R::DHCSR::c_debugen)))) != 0u;
}

using DWT_R = Kvasir::Peripheral::DWT::Registers<>;

// The DWT cycle counter: DWT_CYCCNT counts core cycles while DEMCR.TRCENA and DWT_CTRL.CYCCNTENA are set and wraps to
// zero; Armv7-M ARM DDI0403E C1.6.5 (DEMCR), C1.8.7 (DWT_CTRL), C1.8.8 (DWT_CYCCNT). TRCENA first: the DWT is not
// written while it is off - three apply()s, one apply() would order the stores by address. Each set() is a load, an OR
// and a store; the counter's write covers its register, one store.
[[gnu::always_inline]] inline void enableCycleCounter() {   // inlined: a RAM function may call it
    using namespace Kvasir::Register;
    apply(set(DCB_R::DEMCR::trcena));
    apply(write(DWT_R::CYCCNT::cyccnt, value<std::uint32_t, 0U>()));
    apply(set(DWT_R::CTRL::cyccntena));
}

// The cycle count now: one load, always inlined (an ISR profiler's entry and exit).
[[gnu::always_inline]] inline std::uint32_t cycleCount() {
    using namespace Kvasir::Register;
    return get<0>(apply(read(DWT_R::CYCCNT::cyccnt)));
}
}   // namespace Kvasir::Core::Debug
