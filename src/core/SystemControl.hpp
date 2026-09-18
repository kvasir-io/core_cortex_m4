#pragma once

// From core_cortex_common, the submodule at src/cortex_common.
#include "cortex_common/SystemControl.hpp"

// The configurable fault handlers' priorities (SHPR1), which a Cortex-M0+ does not have. They
// come up at 0 and an application rarely moves them; the actions exist so one that does can.
namespace Kvasir::Nvic {
template<int Priority>
struct MakeAction<Action::SetPriority<Priority>, Index<Interrupt::memoryManagement.index()>>
  : decltype(Detail::setHandlerPriority<Priority,
                                        Interrupt::memoryManagement.index(),
                                        Detail::ScbRegs::SHPR1::pri_4>()) {};

template<int Priority>
struct MakeAction<Action::SetPriority<Priority>, Index<Interrupt::busFault.index()>>
  : decltype(Detail::setHandlerPriority<Priority,
                                        Interrupt::busFault.index(),
                                        Detail::ScbRegs::SHPR1::pri_5>()) {};

template<int Priority>
struct MakeAction<Action::SetPriority<Priority>, Index<Interrupt::usageFault.index()>>
  : decltype(Detail::setHandlerPriority<Priority,
                                        Interrupt::usageFault.index(),
                                        Detail::ScbRegs::SHPR1::pri_6>()) {};
}   // namespace Kvasir::Nvic
