#pragma once

// SimulationController (Engineering Schematic v0.4) - integrated runtime owner.
// Owns config, registry, tick scheduler, state, event bus, logger, and replay.
// OMA Alignment: A3 (Simulation Control)

#include "../config/config_loader.h"
#include "../registry/system_registry.h"
#include "../tick/tick_scheduler.h"
#include "../state/state_manager.h"
#include "../events/event_bus.h"
#include "../logging/simulation_logger.h"
#include "../replay/replay_system.h"

class SimulationController {
public:
    SimulationController();

    bool loadConfig(const std::string& configPath);
    bool initSystems();
    void run(uint64_t maxTicks = 0);

    void stop();
    void shutdown();

    SystemRegistry&     registry();
    EventBus&           eventBus();
    StateManager&       stateManager();
    SimulationLogger&   logger();
    ReplaySystem&       replay();
    const ReplaySystem& replay() const;
    const ConfigLoader& config() const;

private:
    ConfigLoader     config_;
    SystemRegistry   registry_;
    TickScheduler    tickScheduler_;
    StateManager     stateManager_;
    EventBus         eventBus_;
    SimulationLogger logger_;
    ReplaySystem     replay_;
    uint64_t         currentTick_;
    bool             replayWired_;

    void wireReplay();
    void onTick(uint64_t tick);
};
