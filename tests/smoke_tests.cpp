// Ex Origine — Smoke Tests (v0.1)
// These tests verify that every core system can be constructed, initialised,
// updated once, and shut down without errors.
//
// Build and run with any C++17-compatible compiler, e.g.:
//   g++ -std=c++17 -I.. smoke_tests.cpp \
//       ../core/tick_system/tick_manager.cpp \
//       ../systems/energy/energy_core.cpp \
//       ../systems/energy/energy_flow_solver.cpp \
//       ../systems/information/information_core.cpp \
//       ../systems/entity/entity_core.cpp \
//       ../systems/environment/environment_core.cpp \
//       ../systems/interaction/interaction_core.cpp \
//       -o smoke_tests && ./smoke_tests

#include <cassert>
#include <iostream>

#include "../core/tick_system/tick_manager.h"
#include "../systems/energy/energy_core.h"
#include "../systems/energy/energy_flow_solver.h"
#include "../systems/information/information_core.h"
#include "../systems/entity/entity_core.h"
#include "../systems/environment/environment_core.h"
#include "../systems/interaction/interaction_core.h"

static int passed = 0;
static int failed = 0;

#define TEST(name, expr)                                          \
    do {                                                          \
        if (expr) { ++passed; std::cout << "[PASS] " name "\n"; } \
        else      { ++failed; std::cout << "[FAIL] " name "\n"; } \
    } while (false)

// ── TickManager ──────────────────────────────────────────────────────────────
void test_tick_manager() {
    TickManager tm(20.0);
    bool inputFired = false;

    tm.registerPhaseCallback(TickPhase::Input, [&]() { inputFired = true; });
    TEST("TickManager: tick count starts at 0", tm.tickCount() == 0);
    tm.tick();
    TEST("TickManager: tick count increments", tm.tickCount() == 1);
    TEST("TickManager: Input phase callback fires", inputFired);
}

// ── EnergySystem ─────────────────────────────────────────────────────────────
void test_energy_system() {
    EnergySystem es;
    es.init();
    TEST("EnergySystem: total energy starts at 0", es.totalEnergy() == 0.0);
    es.injectEnergy(10.0);
    TEST("EnergySystem: energy increases after injection", es.totalEnergy() == 10.0);
    es.update();
    es.shutdown();
    TEST("EnergySystem: total energy resets on shutdown", es.totalEnergy() == 0.0);
}

// ── EnergyFlowSolver ─────────────────────────────────────────────────────────
void test_energy_flow_solver() {
    EnergyFlowSolver solver(0.5);
    std::vector<double> nodes = {10.0, 0.0};
    solver.solve(nodes);
    TEST("EnergyFlowSolver: energy transfers from high to low node",
         nodes[0] < 10.0 && nodes[1] > 0.0);
    double sum = nodes[0] + nodes[1];
    TEST("EnergyFlowSolver: total energy is conserved",
         sum > 9.99 && sum < 10.01);
}

// ── InformationSystem ────────────────────────────────────────────────────────
void test_information_system() {
    InformationSystem info;
    info.init();
    TEST("InformationSystem: key absent before set", !info.hasState("x"));
    info.setState("x", 42);
    TEST("InformationSystem: key present after set", info.hasState("x"));
    auto val = std::any_cast<int>(info.getState("x"));
    TEST("InformationSystem: stored value retrieved correctly", val == 42);
    info.shutdown();
    TEST("InformationSystem: state cleared on shutdown", !info.hasState("x"));
}

// ── EntitySystem ─────────────────────────────────────────────────────────────
void test_entity_system() {
    EntitySystem es;
    es.init();
    TEST("EntitySystem: no entities on init", es.entities().empty());
    auto id = es.spawnEntity(1.0);
    TEST("EntitySystem: entity spawned", es.entities().size() == 1);
    es.destroyEntity(id);
    es.update();
    TEST("EntitySystem: destroyed entity removed on update", es.entities().empty());
    es.shutdown();
}

// ── EnvironmentSystem ────────────────────────────────────────────────────────
void test_environment_system() {
    EnvironmentSystem env;
    env.init();
    TEST("EnvironmentSystem: no regions on init", env.regions().empty());
    env.addRegion(1.0, 50.0);
    TEST("EnvironmentSystem: region added", env.regions().size() == 1);
    env.update();
    env.shutdown();
    TEST("EnvironmentSystem: regions cleared on shutdown", env.regions().empty());
}

// ── InteractionSystem ────────────────────────────────────────────────────────
void test_interaction_system() {
    InteractionSystem inter;
    inter.init();
    TEST("InteractionSystem: log empty on init", inter.log().empty());
    inter.queueInteraction({"collision", 1, 2, 5.0});
    inter.update();
    TEST("InteractionSystem: queued event appears in log", inter.log().size() == 1);
    inter.shutdown();
    TEST("InteractionSystem: log cleared on shutdown", inter.log().empty());
}

// ── Main ─────────────────────────────────────────────────────────────────────
int main() {
    test_tick_manager();
    test_energy_system();
    test_energy_flow_solver();
    test_information_system();
    test_entity_system();
    test_environment_system();
    test_interaction_system();

    std::cout << "\n" << passed << " passed, " << failed << " failed.\n";
    return failed == 0 ? 0 : 1;
}
