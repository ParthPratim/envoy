#include "extensions/watchdog/profile_action/profile_action.h"

#include <chrono>

#include "envoy/thread/thread.h"

#include "common/profiler/profiler.h"
#include "common/protobuf/utility.h"

#include "absl/strings/str_format.h"

namespace Envoy {
namespace Extensions {
namespace Watchdog {
namespace ProfileAction {
namespace {
static constexpr uint64_t DefaultMaxProfiles = 10;

std::string generateProfileFilePath(const std::string& directory, const SystemTime& now) {
  auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
  if (absl::EndsWith(directory, "/")) {
    return absl::StrFormat("%s%s.%d", directory, "ProfileAction", timestamp);
  }
  return absl::StrFormat("%s/%s.%d", directory, "ProfileAction", timestamp);
}
} // namespace

ProfileAction::ProfileAction(
    envoy::extensions::watchdog::profile_action::v3alpha::ProfileActionConfig& config,
    Server::Configuration::GuardDogActionFactoryContext& context)
    : path_(config.profile_path()),
      duration_(
          std::chrono::milliseconds(PROTOBUF_GET_MS_OR_DEFAULT(config, profile_duration, 5000))),
      max_profiles_(config.max_profiles() == 0 ? DefaultMaxProfiles : config.max_profiles()),
      running_profile_(false), profiles_started_(0), context_(context),
      timer_cb_(context_.dispatcher_.createTimer([this] {
        if (Profiler::Cpu::profilerEnabled()) {
          Profiler::Cpu::stopProfiler();
          running_profile_ = false;
        } else {
          ENVOY_LOG_MISC(error,
                         "Profile Action's stop() was scheduled, but profiler isn't running!");
        }

        if (!context_.api_.fileSystem().fileExists(profile_filename_)) {
          ENVOY_LOG_MISC(error, "Profile file {} wasn't created!", profile_filename_);
        }
      })) {}

void ProfileAction::run(
    envoy::config::bootstrap::v3::Watchdog::WatchdogAction::WatchdogEvent /*event*/,
    const std::vector<std::pair<Thread::ThreadId, MonotonicTime>>& thread_last_checkin_pairs,
    MonotonicTime /*now*/) {
  if (running_profile_) {
    return;
  }

  // Check if there's a tid that justifies profiling
  if (thread_last_checkin_pairs.empty()) {
    ENVOY_LOG_MISC(warn, "Profile Action: No tids were provided.");
    return;
  }

  if (profiles_started_ >= max_profiles_) {
    ENVOY_LOG_MISC(warn,
                   "Profile Action: Unable to profile: enabled but already wrote {} profiles.",
                   profiles_started_);
    return;
  }

  auto& fs = context_.api_.fileSystem();
  if (!fs.directoryExists(path_)) {
    ENVOY_LOG_MISC(error, "Profile Action: Directory path {} doesn't exist.", path_);
    return;
  }

  // Generate file path for output and try to profile
  profile_filename_ = generateProfileFilePath(path_, context_.api_.timeSource().systemTime());

  if (!Profiler::Cpu::profilerEnabled()) {
    if (Profiler::Cpu::startProfiler(profile_filename_)) {
      // Update state
      running_profile_ = true;
      ++profiles_started_;

      // Schedule callback to stop
      timer_cb_->enableTimer(duration_);
    } else {
      ENVOY_LOG_MISC(error, "Profile Action failed to start the profiler.");
    }
  } else {
    ENVOY_LOG_MISC(error, "Profile Action unable to start the profiler as it is in use elsewhere.");
  }
}

} // namespace ProfileAction
} // namespace Watchdog
} // namespace Extensions
} // namespace Envoy
