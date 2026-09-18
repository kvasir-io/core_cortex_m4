#pragma once
#include "core_peripherals/SCB.hpp"
#include "kvasir/Register/Register.hpp"
#include "kvasir/Util/StaticString.hpp"

#include <cstdint>
#include <string_view>
#include <utility>

using Kvasir::Register::apply;
using Kvasir::Register::read;
using Kvasir::Register::write;

// Kvasir::Fault::Core, next to the SDK's Kvasir::Fault (FaultHandler.hpp): its log module is
// "fault.core", derived from the scope.
namespace Kvasir::Fault::Core {
using SCB_R = Kvasir::Peripheral::SCB::Registers<>;

enum class FaultType : std::uint8_t { Hard, Usage, Bus, MemManage };

enum class FaultDescription : std::uint8_t {
    VectorTable,
    UnknownEscalation,
    Unknown,
    DivisionByZero,
    UnalignedAccess,
    NoCoprocessor,
    InvalidPCLoad,
    InvalidState,
    UndefinedInstruction,
    LazyStatePreservationError,
    ExceptionStackingError,
    ExceptionUnstackingError,
    ImpreciseDataAccessError,
    PreciseDataAccessError,
    InstructionBusError,
    DataAccessViolation,
    InstructionAccessViolation
};

struct FaultInfo {
    FaultType                    type;
    FaultDescription             description;
    std::optional<std::uint32_t> fault_address;
    std::uint32_t                status_bits;
    bool                         forced;
};

namespace detail {

    static constexpr void analyze_usage_fault(FaultInfo&    info,
                                              std::uint32_t ufsr) {
        info.status_bits = ufsr;
        info.type        = FaultType::Usage;

        if(ufsr & (1U << 9)) {
            info.description = FaultDescription::DivisionByZero;
            return;
        }
        if(ufsr & (1U << 8)) {
            info.description = FaultDescription::UnalignedAccess;
            return;
        }
        if(ufsr & (1U << 3)) {
            info.description = FaultDescription::NoCoprocessor;
            return;
        }
        if(ufsr & (1U << 2)) {
            info.description = FaultDescription::InvalidPCLoad;
            return;
        }
        if(ufsr & (1U << 1)) {
            info.description = FaultDescription::InvalidState;
            return;
        }
        if(ufsr & (1U << 0)) {
            info.description = FaultDescription::UndefinedInstruction;
            return;
        }
    }

    static constexpr void analyze_bus_fault(FaultInfo&    info,
                                            std::uint32_t bfsr,
                                            std::uint32_t bfar) {
        info.status_bits = bfsr;

        bool const address_valid = bfsr & (1U << 7);
        info.fault_address = address_valid ? std::optional<std::uint32_t>{bfar} : std::nullopt;

        info.type = FaultType::Bus;

        if(bfsr & (1U << 5)) {
            info.description = FaultDescription::LazyStatePreservationError;
            return;
        }
        if(bfsr & (1U << 4)) {
            info.description = FaultDescription::ExceptionStackingError;
            return;
        }
        if(bfsr & (1U << 3)) {
            info.description = FaultDescription::ExceptionUnstackingError;
            return;
        }
        if(bfsr & (1U << 2)) {
            info.description = FaultDescription::ImpreciseDataAccessError;
            return;
        }
        if(bfsr & (1U << 1)) {
            info.description = FaultDescription::PreciseDataAccessError;
            return;
        }
        if(bfsr & (1U << 0)) {
            info.description = FaultDescription::InstructionBusError;
            return;
        }
    }

    static constexpr void analyze_memmanage_fault(FaultInfo&    info,
                                                  std::uint32_t mmfsr,
                                                  std::uint32_t mmfar) {
        info.status_bits = mmfsr;

        bool const address_valid = mmfsr & (1U << 7);
        info.fault_address = address_valid ? std::optional<std::uint32_t>{mmfar} : std::nullopt;

        info.type = FaultType::MemManage;

        if(mmfsr & (1U << 5)) {
            info.description = FaultDescription::LazyStatePreservationError;
            return;
        }
        if(mmfsr & (1U << 4)) {
            info.description = FaultDescription::ExceptionStackingError;
            return;
        }
        if(mmfsr & (1U << 3)) {
            info.description = FaultDescription::ExceptionUnstackingError;
            return;
        }
        if(mmfsr & (1U << 1)) {
            info.description = FaultDescription::DataAccessViolation;
            return;
        }
        if(mmfsr & (1U << 0)) {
            info.description = FaultDescription::InstructionAccessViolation;
            return;
        }
    }

}   // namespace detail

static FaultInfo GetFaultInfo() {
    auto fault_regs = apply(read(SCB_R::CFSR::ufsr),
                            read(SCB_R::CFSR::bfsr),
                            read(SCB_R::CFSR::mmfsr),
                            read(SCB_R::HFSR::debugevt),
                            read(SCB_R::HFSR::forced),
                            read(SCB_R::HFSR::vecttbl),
                            read(SCB_R::MMFAR::address),
                            read(SCB_R::BFAR::address));

    auto ufsr  = get<0>(fault_regs);
    auto bfsr  = get<1>(fault_regs);
    auto mmfsr = get<2>(fault_regs);

    auto hfsr_debugevt = static_cast<std::uint32_t>(get<3>(fault_regs));
    auto hfsr_forced   = static_cast<std::uint32_t>(get<4>(fault_regs));
    auto hfsr_vecttbl  = static_cast<std::uint32_t>(get<5>(fault_regs));

    auto mmfar = get<6>(fault_regs);
    auto bfar  = get<7>(fault_regs);

    FaultInfo info{};
    info.type        = FaultType::Hard;
    info.description = FaultDescription::Unknown;

    if(hfsr_vecttbl) {
        info.description = FaultDescription::VectorTable;
        info.status_bits = hfsr_vecttbl;
        return info;
    }

    info.forced = hfsr_forced != 0;

    if(ufsr) {
        detail::analyze_usage_fault(info, ufsr);
        return info;
    }
    if(bfsr) {
        detail::analyze_bus_fault(info, bfsr, bfar);
        return info;
    }
    if(mmfsr) {
        detail::analyze_memmanage_fault(info, mmfsr, mmfar);
        return info;
    }

    if(info.forced) {
        info.description = FaultDescription::UnknownEscalation;
        info.status_bits = ufsr | bfsr | mmfsr;
        return info;
    }

    info.status_bits = hfsr_debugevt | hfsr_forced | hfsr_vecttbl;
    return info;
}

using EarlyInitList = decltype(MPL::list(
  // Enable fault exceptions
  write(SCB_R::SHCSR::MEMFAULTENAValC::enable_memmanage_fault),
  write(SCB_R::SHCSR::BUSFAULTENAValC::enable_busfault),
  write(SCB_R::SHCSR::USGFAULTENAValC::enable_usagefault),
  // Enable additional fault detection
  write(SCB_R::CCR::DIV_0_TRPValC::trapping_enabled)));

static inline void Log([[maybe_unused]] std::uint32_t const* stack_ptr,
                       [[maybe_unused]] std::uint32_t        lr_value) {
    [[maybe_unused]] FaultInfo const fault_info = GetFaultInfo();

    UC_LOG_C(
      "COREFAULT type({}Fault) forced({}) info({}) flags({:#010x}) address({:#010x}) "
      "registers: PC={:#010x} R0={:#010x} R1={:#010x} R2={:#010x} R3={:#010x} R12={:#010x} "
      "LR={:#010x} "
      "xPSR={:#010x} EXC_RETURN={:#010x}",
      fault_info.type,
      fault_info.forced,
      fault_info.description,
      fault_info.status_bits,
      fault_info.fault_address,
      stack_ptr[6],
      stack_ptr[0],
      stack_ptr[1],
      stack_ptr[2],
      stack_ptr[3],
      stack_ptr[4],
      stack_ptr[5],
      stack_ptr[7],
      lr_value);
}

}   // namespace Kvasir::Fault::Core
