// Ex Origine — Core Unit Tests (v0.1)
// Covers: EventBus, SimulationLogger, SimulationState, StateManager,
//         ConfigLoader, SystemRegistry, TickScheduler,
//         EnergyPool, EnergySystem, StateMemory, InformationSystem,
//         BehaviorModel, EntitySystem, WorldGrid, InteractionSystem.
//
// Build (from repo root):
//   g++ -std=c++17 \
//       tests/unit/core_unit_tests.cpp \
//       core/events/event_bus.cpp \
//       core/logging/simulation_logger.cpp \
//       core/state/simulation_state.cpp \
//       core/state/state_manager.cpp \
//       core/config/config_loader.cpp \
//       core/registry/system_registry.cpp \
//       core/tick/tick_scheduler.cpp \
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
//       -o /tmp/core_unit_tests && /tmp/core_unit_tests

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

#include "../../core/events/event_bus.h"
#include "../../core/logging/simulation_logger.h"
#include "../../core/state/simulation_state.h"
#include "../../core/state/state_manager.h"
#include "../../core/config/config_loader.h"
#include "../../core/registry/system_registry.h"
#include "../../core/tick/tick_scheduler.h"
#include "../../core/isystem.h"

#include "../../systems/energy/energy_pool.h"
#include "../../systems/energy/energy_system.h"
#include "../../systems/information/state_memory.h"
#include "../../systems/information/information_system.h"
#include "../../systems/entity/entity.h"
#include "../../systems/entity/behavior_model.h"
#include "../../systems/entity/entity_system.h"
#include "../../systems/environment/world_grid.h"
#include "../../systems/environment/environment_system.h"
#include "../../systems/interaction/interaction_event.h"
#include "../../systems/interaction/interaction_system.h"

static int passed = 0;
static int failed = 0;

#define TEST(name, expr) \
    do { \
        if (expr) { ++passed; std::cout << "[PASS] " name "\n"; } \
        else      { ++failed; std::cout << "[FAIL] " name "\n"; } \
    } while (false)

// ── EventBus ─────────────────────────────────────────────────────────────────
void test_event_bus() {
    EventBus bus;
    int count = 0;
    bus.subscribe(EventType::EnergyTransfer, [&](const SimEvent& e) { count += (int)e.value; });
    bus.publish({EventType::EnergyTransfer, 1, 0, 3.0, "test"});
    TEST("EventBus: handler receives event", count == 3);
    bus.publish({EventType::EntityAction, 1, 0, 5.0, "other"});
    TEST("EventBus: unsubscribed event is ignored", count == 3);
}

// ── SimulationState ───────────────────────────────────────────────────────────
void test_simulation_state() {
    SimulationState s;
    s.tick = 5;
    TEST("SimulationState: tick assigned", s.tick == 5);
    s.set("x", 42);
    TEST("SimulationState: has key after set", s.has("x"));
    TEST("SimulationState: key absent before set", !s.has("y"));
    auto val = std::any_cast<int>(s.get("x"));
    TEST("SimulationState: stored value correct", val == 42);
}

// ── StateManager ──────────────────────────────────────────────────────────────
void test_state_manager() {
    StateManager sm;
    TEST("StateManager: history empty on creation", sm.historySize() == 0);
    SimulationState s1; s1.tick = 1;
    sm.commit(s1);
    TEST("StateManager: history grows after commit", sm.historySize() == 1);
    TEST("StateManager: current tick is correct", sm.current().tick == 1);
    SimulationState s2; s2.tick = 2;
    sm.commit(s2);
    TEST("StateManager: current updated on second commit", sm.current().tick == 2);
    TEST("StateManager: history has two entries", sm.historySize() == 2);
}

// ── ConfigLoader ──────────────────────────────────────────────────────────────
void test_config_loader() {
    // Write a temporary config file.
    const std::string tmpPath = "/tmp/ex_origine_test_config.json";
    {
        std::ofstream f(tmpPath);
        f << "{\n"
          << "  \"tick_rate_hz\": 30,\n"
          << "  \"enable_logging\": true,\n"
          << "  \"label\": \"test\"\n"
          << "}\n";
    }
    ConfigLoader cl;
    TEST("ConfigLoader: load returns true for valid file", cl.load(tmpPath));
    TEST("ConfigLoader: has() returns true for present key", cl.has("tick_rate_hz"));
    TEST("ConfigLoader: has() returns false for absent key", !cl.has("missing_key"));
    TEST("ConfigLoader: getDouble reads number", cl.getDouble("tick_rate_hz") == 30.0);
    TEST("ConfigLoader: getInt reads number as int", cl.getInt("tick_rate_hz") == 30);
    TEST("ConfigLoader: getBool reads true", cl.getBool("enable_logging") == true);
    TEST("ConfigLoader: getString reads string", cl.getString("label") == "test");
    TEST("ConfigLoader: default returned for missing key",
         cl.getString("missing_key", "default") == "default");
    TEST("ConfigLoader: load returns false for missing file",
         !cl.load("/tmp/no_such_file.json"));
}

// ── SystemRegistry ────────────────────────────────────────────────────────────
struct DummySystem : public ISystem {
    bool inited = false;
    void init()     override { inited = true; }
    void update()   override {}
    void shutdown() override {}
};

void test_system_registry() {
    SystemRegistry reg;
    auto ds = std::make_shared<DummySystem>();
    reg.registerSystem("dummy", ds);
    TEST("SystemRegistry: hasSystem after register", reg.hasSystem("dummy"));
    TEST("SystemRegistry: getSystem returns correct ptr", reg.getSystem("dummy") == ds);
    TEST("SystemRegistry: orderedSystems has one entry", reg.orderedSystems().size() == 1);
    reg.registerSystem("dummy", ds);  // re-registration is a no-op
    TEST("SystemRegistry: duplicate registration ignored", reg.orderedSystems().size() == 1);
    reg.deregisterSystem("dummy");
    TEST("SystemRegistry: hasSystem false after deregister", !reg.hasSystem("dummy"));
}

// ── TickScheduler ─────────────────────────────────────────────────────────────
void test_tick_scheduler() {
    TickScheduler ts(1000.0);  // high rate so 5 ticks complete near-instantly
    int ticks = 0;
    ts.addTickCallback([&](uint64_t) { ++ticks; });
    TEST("TickScheduler: tick count starts at 0", ts.tickCount() == 0);
    ts.run(5);
    TEST("TickScheduler: ran exactly 5 ticks", ticks == 5);
    TEST("TickScheduler: tickCount reports 5", ts.tickCount() == 5);
}

// ── EnergyPool ────────────────────────────────────────────────────────────────
void test_energy_pool() {
    EnergyPool pool(1, 100.0, 50.0);
    TEST("EnergyPool: initial level correct", pool.level() == 50.0);
    TEST("EnergyPool: capacity correct", pool.capacity() == 100.0);
    TEST("EnergyPool: deposit succeeds", pool.deposit(30.0));
    TEST("EnergyPool: level after deposit", pool.level() == 80.0);
    TEST("EnergyPool: withdraw succeeds when sufficient", pool.withdraw(20.0));
    TEST("EnergyPool: level after withdraw", pool.level() == 60.0);
    TEST("EnergyPool: withdraw fails when insufficient", !pool.withdraw(1000.0));
    pool.decay(0.5);
    TEST("EnergyPool: decay reduces level", pool.level() < 60.0);
    TEST("EnergyPool: deposit capped at capacity", pool.deposit(9999.0) && pool.level() == 100.0);
}

// ── EnergySystem ──────────────────────────────────────────────────────────────
void test_energy_system() {
    EventBus bus;
    bool transferFired = false;
    bus.subscribe(EventType::EnergyTransfer, [&](const SimEvent&) { transferFired = true; });

    EnergySystem es(bus, 0.01);
    es.init();
    TEST("EnergySystem: total energy starts at 0", es.totalEnergy() == 0.0);
    uint64_t pid = es.createPool(100.0, 50.0);
    TEST("EnergySystem: total energy after pool creation", es.totalEnergy() == 50.0);
    TEST("EnergySystem: withdraw succeeds", es.withdraw(pid, 10.0));
    TEST("EnergySystem: EnergyTransfer event published on withdraw", transferFired);
    double before = es.totalEnergy();
    es.update();
    TEST("EnergySystem: decay reduces total energy", es.totalEnergy() < before);
    es.shutdown();
    TEST("EnergySystem: total energy zero after shutdown", es.totalEnergy() == 0.0);
}

// ── StateMemory ───────────────────────────────────────────────────────────────
void test_state_memory() {
    StateMemory mem(5);
    TEST("StateMemory: empty on creation", mem.entryCount() == 0);
    mem.record(1, "a", 10);
    TEST("StateMemory: has key after record", mem.has("a"));
    TEST("StateMemory: latest value correct", std::any_cast<int>(mem.latest("a")) == 10);
    // Fill past capacity to test eviction.
    for (int i = 0; i < 5; ++i) mem.record(i + 2, "x", i);
    TEST("StateMemory: history bounded to maxHistory", mem.entryCount() <= 5);
}

// ── InformationSystem ─────────────────────────────────────────────────────────
void test_information_system() {
    EventBus bus;
    InformationSystem info(bus);
    info.init();
    info.recordSignal(0, "pressure", 42.0);
    TEST("InformationSystem: signal recorded", info.hasSignal("pressure"));
    TEST("InformationSystem: signal value correct",
         std::any_cast<double>(info.latestSignal("pressure")) == 42.0);
    info.shutdown();
}

// ── BehaviorModel ─────────────────────────────────────────────────────────────
void test_behavior_model() {
    BehaviorModel bm;
    Entity alive{1, 10.0, EntityState::Idle, "test"};
    auto res = bm.evaluate(alive);
    TEST("BehaviorModel: alive entity with energy acts", res.acted);
    TEST("BehaviorModel: action has positive cost", res.energyCost > 0.0);

    Entity dead{2, 0.0, EntityState::Dead, "dead"};
    auto res2 = bm.evaluate(dead);
    TEST("BehaviorModel: dead entity does not act", !res2.acted);

    Entity starved{3, 0.001, EntityState::Idle, "starved"};
    auto res3 = bm.evaluate(starved);
    TEST("BehaviorModel: entity below threshold does not act", !res3.acted);
}

// ── EntitySystem ──────────────────────────────────────────────────────────────
void test_entity_system() {
    EventBus bus;
    EnergySystem es(bus, 0.0);
    es.init();
    uint64_t poolId = es.createPool(100.0, 100.0);

    EntitySystem entsys(bus, es);
    entsys.init();
    TEST("EntitySystem: no entities on init", entsys.entityCount() == 0);

    entsys.spawnEntity(50.0, poolId, "agent");
    TEST("EntitySystem: entity spawned", entsys.entityCount() == 1);

    entsys.update();
    TEST("EntitySystem: entity still alive after one tick", entsys.entityCount() == 1);

    entsys.shutdown();
    TEST("EntitySystem: cleared on shutdown", entsys.entityCount() == 0);
    es.shutdown();
}

// ── WorldGrid ─────────────────────────────────────────────────────────────────
void test_world_grid() {
    WorldGrid grid(5, 5);
    TEST("WorldGrid: width correct", grid.width() == 5);
    TEST("WorldGrid: height correct", grid.height() == 5);
    grid.at(0, 0).resource = 10.0f;
    grid.at(1, 0).resource = 0.0f;
    float before = grid.at(0, 0).resource;
    grid.diffuse(0.5f);
    TEST("WorldGrid: diffusion transfers resource to neighbour",
         grid.at(0, 0).resource < before && grid.at(1, 0).resource > 0.0f);
}

// ── InteractionSystem ─────────────────────────────────────────────────────────
void test_interaction_system() {
    EventBus bus;
    bool resolved = false;
    bus.subscribe(EventType::InteractionResolved, [&](const SimEvent&) { resolved = true; });

    InteractionSystem inter(bus);
    inter.init();
    TEST("InteractionSystem: log empty on init", inter.resolvedLog().empty());
    inter.queue({"collision", 1, 2, 5.0, false});
    inter.update();
    TEST("InteractionSystem: event in resolved log", inter.resolvedLog().size() == 1);
    TEST("InteractionSystem: InteractionResolved event published", resolved);
    inter.shutdown();
    TEST("InteractionSystem: log cleared on shutdown", inter.resolvedLog().empty());
}

// ── Main ──────────────────────────────────────────────────────────────────────
int main() {
    test_event_bus();
    test_simulation_state();
    test_state_manager();
    test_config_loader();
    test_system_registry();
    test_tick_scheduler();
    test_energy_pool();
    test_energy_system();
    test_state_memory();
    test_information_system();
    test_behavior_model();
    test_entity_system();
    test_world_grid();
    test_interaction_system();

    std::cout << "\n" << passed << " passed, " << failed << " failed.\n";
    return failed == 0 ? 0 : 1;
}
