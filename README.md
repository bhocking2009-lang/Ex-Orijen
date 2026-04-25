# Ex Origine

**Ex Origine** is a modular real-time simulation framework that models the emergence of complexity from first principles.

It is not simply a game — it is a system where physics, information, and decision-making co-evolve, allowing players to guide, observe, or influence the unfolding of structured reality from its most primitive states.

> *"What happens when a world is allowed to become?"*

---

## Project Status

`v0.1 — Architectural skeleton established`

---

## Repository Layout

```
docs/
  vision/          — Project vision & narrative
  architecture/    — Wireframes and structural specs
  systems/         — Per-system design documents
  engineering/     — Engineering implementation schematics

core/
  app/             — Application entry (main.cpp + Application class)
  config/          — ConfigLoader (zero-dependency JSON reader)
  simulation/      — Integrated SimulationController (Engineering v0.1)
  tick/            — TickScheduler (fixed-timestep, chrono-based)
  registry/        — SystemRegistry (ordered, enable/disable)
  state/           — SimulationState + StateManager
  events/          — EventBus + EventTypes
  logging/         — SimulationLogger
  replay/          — ReplaySystem (event recording + save)
  simulation_controller/  — v0.1 skeleton SimulationController (preserved)
  tick_system/            — v0.1 skeleton TickManager (preserved)

systems/
  energy/          — EnergyPool + EnergySystem (+ legacy energy_core)
  information/     — StateMemory + InformationSystem (+ legacy info_core)
  entity/          — Entity + BehaviorModel + EntitySystem (+ legacy entity_core)
  environment/     — WorldGrid + EnvironmentSystem (+ legacy environment_core)
  interaction/     — InteractionEvent + InteractionSystem (+ legacy interaction_core)

data/
  configs/         — simulation_config.json, system_defaults.json
  schemas/         — JSON schemas for all data files
  rules/           — (future) data-driven rule files
  seeds/           — Initial world conditions

runtime/
  logs/            — Simulation event logs
  saves/           — Serialised simulation snapshots
  replays/         — Deterministic replay recordings

interface/
  visualization/   — Output / rendering layer
  controls/        — Player input handling

tests/
  smoke_tests.cpp          — v0.1 skeleton smoke tests (21 assertions)
  unit/core_unit_tests.cpp — Engineering v0.1 unit tests (64 assertions)
  integration/mvb_integration_test.cpp — Full MVB integration test (8 assertions)
```

---

## Design Pillars

- **Everything is derived** — no arbitrary constants; all values emerge from rules.
- **Everything interacts** — every system affects every other through defined interfaces.
- **Everything persists** — all state changes are logged and replayable.
- **Everything evolves** — systems are versioned and replaceable without rewrites.

---

## Documentation

| Document | Path |
|---|---|
| Project Vision (v0.1) | `docs/vision/ex_origine_project_statement_v0.1.txt` |
| Architecture Wireframe (v0.1) | `docs/architecture/project_wireframe_v0.1.txt` |
| Program Engineer Schematic (v0.1) | `docs/engineering/program_engineer_schematic_v0.1.txt` |

---

## Building & Running

Requires a C++17-compatible compiler. All commands run from the repo root.

### MVB Executable (Engineering Schematic v0.1)

```sh
g++ -std=c++17 -I. \
    core/app/main.cpp core/app/application.cpp \
    core/events/event_bus.cpp \
    core/logging/simulation_logger.cpp \
    core/state/simulation_state.cpp core/state/state_manager.cpp \
    core/config/config_loader.cpp \
    core/registry/system_registry.cpp \
    core/tick/tick_scheduler.cpp \
    core/simulation/simulation_controller.cpp \
    systems/energy/energy_pool.cpp systems/energy/energy_system.cpp \
    systems/information/state_memory.cpp systems/information/information_system.cpp \
    systems/entity/entity.cpp systems/entity/behavior_model.cpp systems/entity/entity_system.cpp \
    systems/environment/world_grid.cpp systems/environment/environment_system.cpp \
    systems/interaction/interaction_event.cpp systems/interaction/interaction_system.cpp \
    -o ex_origine && ./ex_origine
```

Optional flags: `--config <path>` `--ticks <n>`

### Unit Tests (64 assertions)

```sh
g++ -std=c++17 -I. \
    tests/unit/core_unit_tests.cpp \
    core/events/event_bus.cpp core/logging/simulation_logger.cpp \
    core/state/simulation_state.cpp core/state/state_manager.cpp \
    core/config/config_loader.cpp core/registry/system_registry.cpp \
    core/tick/tick_scheduler.cpp \
    systems/energy/energy_pool.cpp systems/energy/energy_system.cpp \
    systems/information/state_memory.cpp systems/information/information_system.cpp \
    systems/entity/entity.cpp systems/entity/behavior_model.cpp systems/entity/entity_system.cpp \
    systems/environment/world_grid.cpp systems/environment/environment_system.cpp \
    systems/interaction/interaction_event.cpp systems/interaction/interaction_system.cpp \
    -o unit_tests && ./unit_tests
```

### MVB Integration Test (8 assertions)

```sh
g++ -std=c++17 -I. \
    tests/integration/mvb_integration_test.cpp \
    core/events/event_bus.cpp core/logging/simulation_logger.cpp \
    core/state/simulation_state.cpp core/state/state_manager.cpp \
    core/config/config_loader.cpp core/registry/system_registry.cpp \
    core/tick/tick_scheduler.cpp core/simulation/simulation_controller.cpp \
    systems/energy/energy_pool.cpp systems/energy/energy_system.cpp \
    systems/information/state_memory.cpp systems/information/information_system.cpp \
    systems/entity/entity.cpp systems/entity/behavior_model.cpp systems/entity/entity_system.cpp \
    systems/environment/world_grid.cpp systems/environment/environment_system.cpp \
    systems/interaction/interaction_event.cpp systems/interaction/interaction_system.cpp \
    -o mvb_test && ./mvb_test
```

### v0.1 Skeleton Smoke Tests (21 assertions)

```sh
g++ -std=c++17 \
    tests/smoke_tests.cpp \
    core/tick_system/tick_manager.cpp \
    systems/energy/energy_core.cpp \
    systems/energy/energy_flow_solver.cpp \
    systems/information/information_core.cpp \
    systems/entity/entity_core.cpp \
    systems/environment/environment_core.cpp \
    systems/interaction/interaction_core.cpp \
    -o smoke_tests && ./smoke_tests
```

---

## OMA Layer Alignment

| Layer | Responsibility |
|---|---|
| A1 | Foundational intent (vision documents) |
| A2 | Core systems: Energy, Information, Entity |
| A3 | Simulation control: Tick Manager, Scheduler |
| A4 | Player interface: Observer / Influencer / Architect |
| A5 | Data layer: Persistence, Logging, Replay, Versioning |
