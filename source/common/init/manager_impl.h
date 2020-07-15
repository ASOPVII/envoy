#pragma once

#include <list>

#include "envoy/init/manager.h"

#include "common/common/logger.h"
#include "common/init/watcher_impl.h"

#include "absl/container/flat_hash_map.h"

namespace Envoy {
namespace Init {

/**
 * Init::ManagerImpl coordinates initialization of one or more "targets." See comments in
 * include/envoy/init/manager.h for an overview.
 *
 * When the logging level is set to "debug" or "trace," the log will contain entries for all
 * significant events in the initialization flow:
 *
 *   - Targets added to the manager
 *   - Initialization started for the manager and for each target
 *   - Initialization completed for each target and for the manager
 *   - Destruction of targets and watchers
 *   - Callbacks to "unavailable" (deleted) targets, manager, or watchers
 */
class ManagerImpl : public Manager, Logger::Loggable<Logger::Id::init> {
public:
  /**
   * @param name a human-readable manager name, for logging / debugging.
   */
  ManagerImpl(absl::string_view name);

  // Init::Manager
  State state() const override;
  void add(const Target& target) override;
  void initialize(const Watcher& watcher) override;

  const absl::flat_hash_map<std::string, uint32_t>& unreadyTargets() const;

private:
  void onTargetReady();
  void ready();
  void onTargetReadySendTargetName(absl::string_view target_name);

  // Human-readable name for logging
  const std::string name_;

  // Current state
  State state_;

  // Current number of registered targets that have not yet initialized
  uint32_t count_;

  // Handle to the watcher passed in `initialize`, to be called when initialization completes
  WatcherHandlePtr watcher_handle_;

  // Watcher to receive ready notifications from each target
  const WatcherImpl watcher_;

  // All registered targets
  std::list<TargetHandlePtr> target_handles_;

  // Count of target_name of unready targets.
  absl::flat_hash_map<std::string, uint32_t> target_names_count_;
};

} // namespace Init
} // namespace Envoy
