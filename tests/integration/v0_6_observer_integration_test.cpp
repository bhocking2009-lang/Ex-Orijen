// Ex Origine - v0.2-v0.6 observer integration test.
// Exercises observable snapshots, player influence, contextual entity behavior, and replay inspection.

#include <iostream>
#include <memory>
#include <string>

#include "../../core/simulation/simulation_controller.h"
#include "../../core/influence/player_influence.h"
#include "../../interface/visualization/world_observer.h"
#include "../../interface/visualization/terminal_observer.h"
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

void test_observable_world_loop() {
    SimulationController ctrl;
    TEST("v0.6: config loaded", ctrl.loadConfig("data/configs/simulation_config.json"));

    auto& bus = ctrl.eventBus();
    auto energySys = std::make_shared<EnergySystem>(bus, 0.001);
    auto infoSys   = std::make_shared<InformationSystem>(bus);
    auto entitySys = std::make_shared<EntitySystem>(bus, *energySys);
    auto envSys    = std::make_shared<EnvironmentSystem>(bus, 6, 6);
    auto interSys  = std::make_shared<InteractionSystem>(bus);

    envSys->setInfluenceMode(PlayerInfluenceMode::Attract);
    entitySys->attachEnvironment(*envSys);
    entitySys->setInfluenceMode(PlayerInfluenceMode::Attract);

    ctrl.registry().registerSystem("energy", energySys);
    ctrl.registry().registerSystem("information", infoSys);
    ctrl.registry().registerSystem("entity", entitySys);
    ctrl.registry().registerSystem("environment", envSys);
    ctrl.registry().registerSystem("interaction", interSys);
    ctrl.initSystems();

    WorldObserver observer(bus, *energySys, *entitySys, *envSys);

    uint64_t poolId = energySys->createPool(100.0, 100.0);
    entitySys->spawnEntity(10.0, poolId, "observer-test");

    ctrl.run(3);

    TEST("v0.6: one snapshot per tick", observer.snapshots().size() == 3);
    const WorldSnapshot* latest = observer.latest();
    TEST("v0.6: latest snapshot exists", latest != nullptr);
    TEST("v0.6: snapshot has resources", latest && latest->resources.size() == 36);
    TEST("v0.6: snapshot has entity projection", latest && latest->entityCount == 1 && !latest->entities.empty());
    TEST("v0.6: influence mode captured", latest && latest->influenceMode == "attract");
    TEST("v0.6: entity action becomes visible", latest && !latest->entities.front().lastAction.empty());

    TerminalObserver terminal;
    std::string rendered = latest ? terminal.render(*latest) : "";
    TEST("v0.6: terminal render includes observer header", rendered.find("[OBSERVE]") != std::string::npos);
    TEST("v0.6: terminal render includes entity marker", rendered.find('E') != std::string::npos);

    ReplaySummary summary = ctrl.replay().summary();
    TEST("v0.4: replay captured ticks", summary.tickCount == 3);
    TEST("v0.4: replay captured events", summary.eventCount > 0);
    TEST("v0.4: replay inspect names events", ctrl.replay().inspectTick(0).find("TickBegin") != std::string::npos);

    ctrl.shutdown();
}

int main() {
    test_observable_world_loop();
    std::cout << "\n" << passed << " passed, " << failed << " failed.\n";
    return failed == 0 ? 0 : 1;
}
