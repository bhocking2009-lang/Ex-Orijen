// Ex Origine - v1.0 civilization integration test.
// Exercises player-facing civic decisions, settlement state, ecological feedback,
// observable snapshots, terminal rendering, and replay inspection.

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
#include "../../systems/civilization/civilization_system.h"
#include "../../systems/interaction/interaction_system.h"

static int passed = 0;
static int failed = 0;

#define TEST(name, expr) \
    do { \
        if (expr) { ++passed; std::cout << "[PASS] " name "\n"; } \
        else      { ++failed; std::cout << "[FAIL] " name "\n"; } \
    } while (false)

void test_civilization_world_loop() {
    SimulationController ctrl;
    TEST("v1.0: config loaded", ctrl.loadConfig("data/configs/simulation_config.json"));

    auto& bus = ctrl.eventBus();
    int decisionEvents = 0;
    int settlementEvents = 0;
    int landEvents = 0;
    int seasonEvents = 0;

    bus.subscribe(EventType::CivilizationDecisionApplied, [&](const SimEvent&) { ++decisionEvents; });
    bus.subscribe(EventType::SettlementUpdated, [&](const SimEvent&) { ++settlementEvents; });
    bus.subscribe(EventType::LandUseChanged, [&](const SimEvent&) { ++landEvents; });
    bus.subscribe(EventType::SeasonAdvanced, [&](const SimEvent&) { ++seasonEvents; });

    auto energySys = std::make_shared<EnergySystem>(bus, 0.001);
    auto infoSys   = std::make_shared<InformationSystem>(bus);
    auto entitySys = std::make_shared<EntitySystem>(bus, *energySys);
    auto envSys    = std::make_shared<EnvironmentSystem>(bus, 6, 6);
    auto civSys    = std::make_shared<CivilizationSystem>(bus, *envSys);
    auto interSys  = std::make_shared<InteractionSystem>(bus);

    envSys->setInfluenceMode(PlayerInfluenceMode::Observer);
    entitySys->attachEnvironment(*envSys);
    entitySys->setInfluenceMode(PlayerInfluenceMode::Observer);
    civSys->setDecision(CivicDecision::ClearForest);

    ctrl.registry().registerSystem("energy", energySys);
    ctrl.registry().registerSystem("information", infoSys);
    ctrl.registry().registerSystem("entity", entitySys);
    ctrl.registry().registerSystem("environment", envSys);
    ctrl.registry().registerSystem("civilization", civSys);
    ctrl.registry().registerSystem("interaction", interSys);
    ctrl.initSystems();

    WorldObserver observer(bus, *energySys, *entitySys, *envSys, civSys.get());

    const float forestBefore = civSys->averageForestCover();
    const uint64_t settlementId = civSys->foundSettlement("Delta Reach", 3, 3, 120.0);
    TEST("v1.0: settlement founded", settlementId != 0 && civSys->settlements().size() == 1);

    uint64_t poolId = energySys->createPool(120.0, 120.0);
    entitySys->spawnEntity(12.0, poolId, "civilization-test");

    ctrl.run(4);

    TEST("v1.0: one snapshot per tick", observer.snapshots().size() == 4);
    const WorldSnapshot* latest = observer.latest();
    TEST("v1.0: latest snapshot exists", latest != nullptr);
    TEST("v1.0: snapshot has civilization decision", latest && latest->civicDecision == "clear_forest");
    TEST("v1.0: snapshot has settlement projection", latest && latest->settlements.size() == 1);
    TEST("v1.0: settlement population remains viable", latest && latest->settlements.front().population > 1.0);
    TEST("v1.0: settlement resources are tracked", latest && latest->settlements.front().food >= 0.0 && latest->settlements.front().water >= 0.0);
    TEST("v1.0: season and rainfall are visible", latest && !latest->season.empty() && latest->rainfallIndex > 0.0f);
    TEST("v1.0: food production index is visible", latest && latest->foodProductionIndex > 0.0f);
    TEST("v1.0: clearing forest changes land state", latest && latest->averageForestCover < forestBefore);
    TEST("v1.0: land pressure begins accumulating", latest && latest->averageDesertification > 0.0f);

    TerminalObserver terminal;
    std::string rendered = latest ? terminal.render(*latest) : "";
    TEST("v1.0: terminal render includes settlement marker", rendered.find('S') != std::string::npos);
    TEST("v1.0: terminal render includes settlement summary", rendered.find("settlement#") != std::string::npos);

    TEST("v1.0: decision events fired", decisionEvents >= 4);
    TEST("v1.0: settlement events fired", settlementEvents >= 4);
    TEST("v1.0: land events fired", landEvents >= 4);
    TEST("v1.0: season events fired", seasonEvents >= 4);

    ReplaySummary summary = ctrl.replay().summary();
    const std::string inspected = ctrl.replay().inspectTick(0);
    TEST("v1.0: replay captured civilization events", summary.eventCount > 0);
    TEST("v1.0: replay names civilization decisions", inspected.find("CivilizationDecisionApplied") != std::string::npos);
    TEST("v1.0: replay names settlement updates", inspected.find("SettlementUpdated") != std::string::npos);

    ctrl.shutdown();
}

void test_decision_consequences() {
    EventBus clearBus;
    EnvironmentSystem clearEnv(clearBus, 4, 4);
    CivilizationSystem clearCiv(clearBus, clearEnv);
    clearEnv.init();
    clearCiv.init();
    clearCiv.foundSettlement("Clear Test", 2, 2, 80.0);
    const float clearForestBefore = clearCiv.averageForestCover();
    clearCiv.setDecision(CivicDecision::ClearForest);
    clearCiv.update();
    TEST("v1.0: clear forest lowers forest cover", clearCiv.averageForestCover() < clearForestBefore);

    EventBus rotateBus;
    EnvironmentSystem rotateEnv(rotateBus, 4, 4);
    CivilizationSystem rotateCiv(rotateBus, rotateEnv);
    rotateEnv.init();
    rotateCiv.init();
    rotateCiv.foundSettlement("Rotation Test", 2, 2, 80.0);
    rotateCiv.setDecision(CivicDecision::ExpandFarms);
    rotateCiv.update();
    const float rotationSoilBefore = rotateCiv.averageSoilFertility();
    rotateCiv.setDecision(CivicDecision::RotateCrops);
    rotateCiv.update();
    TEST("v1.0: crop rotation improves depleted soil", rotateCiv.averageSoilFertility() > rotationSoilBefore);

    TEST("v1.0: decision parser accepts player terms", CivilizationSystem::parseDecision("deforest") == CivicDecision::ClearForest);
    TEST("v1.0: decision parser defaults safely", CivilizationSystem::parseDecision("unknown") == CivicDecision::Stewardship);
}

int main() {
    test_civilization_world_loop();
    test_decision_consequences();
    std::cout << "\n" << passed << " passed, " << failed << " failed.\n";
    return failed == 0 ? 0 : 1;
}
