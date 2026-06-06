#pragma once

// SimulationController (Engineering Schematic v0.1) — integrated runtime owner.
// Owns config, registry, tick scheduler, state, event bus, and logger.
// OMA Alignment: A3 (Simulation Control)
//
// Usage:
//   controller.loadConfig("data/configs/simulation_config.json");
//   controller.registry().registerSystem("energy", energySys);
//   controller.initSystems();
//   controller.run(100);
//   controller.shutdown();

#include "../config/config_loader.h"
#include "../registry/system_registry.h"
#include "../tick/tick_scheduler.h"
#include "../state/state_manager.h"
#include "../events/event_bus.h"
#include "../logging/simulation_logger.h"

class SimulationController {
public:
    SimulationController();

    // Step 1: load configuration (must be called before initSystems).
    bool loadConfig(const std::string& configPath);

    // Step 2: initialise all registered systems and wire the tick callback.
    bool initSystems();

    // Step 3: run the tick loop.
    void run(uint64_t maxTicks = 0);

    void stop();
    void shutdown();

    // Accessors used during setup (before initSystems).
    SystemRegistry&     registry();
    EventBus&           eventBus();
    StateManager&       stateManager();
    SimulationLogger&   logger();
    const ConfigLoader& config() const;

private:
    ConfigLoader     config_;
    SystemRegistry   registry_;
    TickScheduler    tickScheduler_;
    StateManager     stateManager_;
    EventBus         eventBus_;
    SimulationLogger logger_;

    void onTick(uint64_t tick);
};
