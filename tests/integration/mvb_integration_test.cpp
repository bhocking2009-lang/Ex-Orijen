// Ex Origine — MVB Integration Test (v0.1)
// Exercises the full minimum viable build:
//   Application starts → config loads → tick loop runs →
//   energy decays → entity acts → state committed → clean exit.
//
// Build (from repo root):
//   g++ -std=c++17 \
//       tests/integration/mvb_integration_test.cpp \
//       core/events/event_bus.cpp \
//       core/logging/simulation_logger.cpp \
//       core/state/simulation_state.cpp \
//       core/state/state_manager.cpp \
//       core/config/config_loader.cpp \
//       core/registry/system_registry.cpp \
//       core/tick/tick_scheduler.cpp \
//       core/simulation/simulation_controller.cpp \
//       systems/energy/energy_pool.cpp \
//       systems/energy/energy_system.cpp \
//       systems/information/state_memory.cpp \
//       systems/information/information_system.cpp \
//       systems/entity/entity.cpp \
//       systems/entity/behavior_model.cpp \
//       systems/entity/entity_system.cpp \
//       systems/environment/world_grid.cpp \
//       systems/environment/environment_system.cpp \
//       systems/interaction/interaction_event.cpp \
//       systems/interaction/interaction_system.cpp \
//       -o /tmp/mvb_integration_test && /tmp/mvb_integration_test

#include <cassert>
#include <iostream>
#include <memory>

#include "../../core/simulation/simulation_controller.h"
#include "../../core/events/event_bus.h"
#include "../../systems/energy/energy_system.h"
#include "../../systems/information/information_system.h"
#include "../../systems/entity/entity_system.h"
#include "../../systems/environment/environment_system.h"
#include "../../systems/interaction/interaction_system.h"

static int passed = 0;
static int failed = 0;

#define TEST(name, expr) \
    do { \
        if (expr) { ++passed; std::cout << "[PASS] " name "\n"; } \
        else      { ++failed; std::cout << "[FAIL] " name "\n"; } \
    } while (false)

void test_mvb_simulation_loop() {
    SimulationController ctrl;

    // Load real config file (cwd must be repo root when running).
    bool loaded = ctrl.loadConfig("data/configs/simulation_config.json");
    TEST("MVB: config loaded", loaded);
    TEST("MVB: tick_rate_hz read from config",
         ctrl.config().getDouble("tick_rate_hz", 0.0) > 0.0);

    auto& bus = ctrl.eventBus();

    // Track events fired during the run.
    int tickBeginCount   = 0;
    int tickEndCount     = 0;
    int energyXferCount  = 0;
    int entityActionCount = 0;

    bus.subscribe(EventType::TickBegin,      [&](const SimEvent&) { ++tickBeginCount; });
    bus.subscribe(EventType::TickEnd,        [&](const SimEvent&) { ++tickEndCount; });
    bus.subscribe(EventType::EnergyTransfer, [&](const SimEvent&) { ++energyXferCount; });
    bus.subscribe(EventType::EntityAction,   [&](const SimEvent&) { ++entityActionCount; });

    // Wire systems.
    auto energySys = std::make_shared<EnergySystem>(bus, 0.001);
    auto infoSys   = std::make_shared<InformationSystem>(bus);
    auto entitySys = std::make_shared<EntitySystem>(bus, *energySys);
    auto envSys    = std::make_shared<EnvironmentSystem>(bus, 5, 5);
    auto interSys  = std::make_shared<InteractionSystem>(bus);

    ctrl.registry().registerSystem("energy",      energySys);
    ctrl.registry().registerSystem("information", infoSys);
    ctrl.registry().registerSystem("entity",      entitySys);
    ctrl.registry().registerSystem("environment", envSys);
    ctrl.registry().registerSystem("interaction", interSys);

    ctrl.initSystems();

    // Seed world.
    uint64_t poolId = energySys->createPool(100.0, 100.0);
    entitySys->spawnEntity(50.0, poolId, "pioneer");

    double energyBefore = energySys->totalEnergy();

    // Run 5 ticks at 1000 Hz so the test completes in milliseconds.
    TickScheduler fastScheduler(1000.0);
    fastScheduler.addTickCallback([&](uint64_t tick) {
        bus.publish({EventType::TickBegin, 0, 0, static_cast<double>(tick), {}});
        for (auto& [name, sys] : ctrl.registry().orderedSystems()) {
            if (sys) sys->update();
        }
        SimulationState s; s.tick = tick;
        ctrl.stateManager().commit(std::move(s));
        bus.publish({EventType::TickEnd, 0, 0, static_cast<double>(tick), {}});
    });
    fastScheduler.run(5);

    TEST("MVB: tick begin events fired for each tick", tickBeginCount == 5);
    TEST("MVB: tick end events fired for each tick",   tickEndCount   == 5);
    TEST("MVB: energy decreased due to decay and entity actions",
         energySys->totalEnergy() < energyBefore);
    TEST("MVB: entity action events were published",   entityActionCount > 0);
    TEST("MVB: state history recorded",
         ctrl.stateManager().historySize() == 5);
    TEST("MVB: entity survived (has energy reserve > 0)",
         entitySys->entityCount() == 1);

    ctrl.shutdown();
}

int main() {
    test_mvb_simulation_loop();
    std::cout << "\n" << passed << " passed, " << failed << " failed.\n";
    return failed == 0 ? 0 : 1;
}
