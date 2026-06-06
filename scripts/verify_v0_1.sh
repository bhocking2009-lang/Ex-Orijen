#!/usr/bin/env sh
set -eu

CXX=${CXX:-g++}
STD=${STD:--std=c++17}
OUT_DIR=${OUT_DIR:-build/v0_6_verify}

mkdir -p "$OUT_DIR"

COMMON_SOURCES="
core/events/event_bus.cpp
core/logging/simulation_logger.cpp
core/state/simulation_state.cpp
core/state/state_manager.cpp
core/config/config_loader.cpp
core/registry/system_registry.cpp
core/tick/tick_scheduler.cpp
core/replay/replay_system.cpp
core/influence/player_influence.cpp
systems/energy/energy_pool.cpp
systems/energy/energy_system.cpp
systems/information/state_memory.cpp
systems/information/information_system.cpp
systems/entity/entity.cpp
systems/entity/behavior_model.cpp
systems/entity/entity_system.cpp
systems/environment/world_grid.cpp
systems/environment/environment_system.cpp
systems/interaction/interaction_event.cpp
systems/interaction/interaction_system.cpp
interface/visualization/world_snapshot.cpp
interface/visualization/terminal_observer.cpp
interface/visualization/world_observer.cpp
"

APP_SOURCES="
core/app/main.cpp
core/app/application.cpp
core/simulation/simulation_controller.cpp
$COMMON_SOURCES
"

UNIT_SOURCES="
tests/unit/core_unit_tests.cpp
$COMMON_SOURCES
"

MVB_TEST_SOURCES="
tests/integration/mvb_integration_test.cpp
core/simulation/simulation_controller.cpp
$COMMON_SOURCES
"

OBSERVER_TEST_SOURCES="
tests/integration/v0_6_observer_integration_test.cpp
core/simulation/simulation_controller.cpp
$COMMON_SOURCES
"

SMOKE_SOURCES="
tests/smoke_tests.cpp
core/tick_system/tick_manager.cpp
systems/energy/energy_core.cpp
systems/energy/energy_flow_solver.cpp
systems/information/information_core.cpp
systems/entity/entity_core.cpp
systems/environment/environment_core.cpp
systems/interaction/interaction_core.cpp
"

echo "[verify] Building MVB executable"
$CXX $STD -I. $APP_SOURCES -o "$OUT_DIR/ex_origine"

echo "[verify] Running default 10-tick MVB"
"$OUT_DIR/ex_origine" --ticks 10

echo "[verify] Running 100-tick observable loop"
"$OUT_DIR/ex_origine" --ticks 100 --mode attract --snapshot runtime/replays/verify_world.jsonl --replay runtime/replays/verify_events.replay

echo "[verify] Checking missing config fails"
if "$OUT_DIR/ex_origine" --config data/configs/no_such_config.json --ticks 1; then
    echo "[verify] Expected missing config to fail"
    exit 1
fi

echo "[verify] Building and running unit tests"
$CXX $STD -I. $UNIT_SOURCES -o "$OUT_DIR/core_unit_tests"
"$OUT_DIR/core_unit_tests"

echo "[verify] Building and running MVB integration test"
$CXX $STD -I. $MVB_TEST_SOURCES -o "$OUT_DIR/mvb_integration_test"
"$OUT_DIR/mvb_integration_test"

echo "[verify] Building and running v0.6 observer integration test"
$CXX $STD -I. $OBSERVER_TEST_SOURCES -o "$OUT_DIR/v0_6_observer_integration_test"
"$OUT_DIR/v0_6_observer_integration_test"

echo "[verify] Building and running legacy smoke tests"
$CXX $STD -I. $SMOKE_SOURCES -o "$OUT_DIR/smoke_tests"
"$OUT_DIR/smoke_tests"

echo "[verify] v0.2-v0.6 closeout checks passed"
