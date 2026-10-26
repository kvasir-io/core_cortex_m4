#pragma once
// The image config header as a test fixture, for the paired chip (an STM32F413): what a firmware's kvasir_config.hpp
// holds - the log backend and a complete log clock. uc_log/uc_log.hpp reads it first (UC_LOG_CONFIG_HEADER, set by
// kvasir_conformance_project), and every conformance TU and every header-alone TU starts with it. Like every config
// header it never includes uc_log/uc_log.hpp. No board: nothing here names pins or a probe.

#include <chip/WaitBounds.hpp>   // Register::CpuHzCeiling<Tag::User>: the loss policy's CPU ceiling
#include <uc_log/AlwaysAttached.hpp>
#include <uc_log/LogClock.hpp>
#include <uc_log/RttLog.hpp>

namespace uc_log {
template<>
struct ComBackend<uc_log::Tag::User>
  : public uc_log::RttLog<
      uc_log::AlwaysAttached,
      uc_log::Rings<1024>,
      uc_log::IsrPolicy::MaskedRecord,
      uc_log::LossPolicy::Counted<Kvasir::Register::CpuHzCeiling<Kvasir::Tag::User>::value>> {};
}   // namespace uc_log

#include <core/Systick.hpp>

namespace Conformance {
inline constexpr auto clockSpeedValue = 16'000'000;

struct SystickClockConfig {
    static constexpr auto clockBase      = Kvasir::Systick::useProcessorClock;
    static constexpr auto clockSpeed     = clockSpeedValue;
    static constexpr auto minOverrunTime = std::chrono::years(15);
};

using SystickClock = Kvasir::Systick::SystickClockBase<SystickClockConfig>;
}   // namespace Conformance

namespace uc_log {
template<>
struct LogClock<uc_log::Tag::User> : public Conformance::SystickClock {};
}   // namespace uc_log
