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

core/
  simulation_controller/   — Top-level lifecycle manager
  tick_system/             — Canonical tick-phase loop

systems/
  energy/          — Energy conservation, flow, and decay
  information/     — Global state store and pattern memory
  entity/          — Agent perception, decision, and action
  environment/     — Spatial structure and resource distribution
  interaction/     — Cross-system effect resolution

data/
  configs/         — Runtime configuration files
  schemas/         — JSON schemas for all data files
  seeds/           — Initial world conditions

runtime/
  logs/            — Simulation event logs
  saves/           — Serialised simulation snapshots
  replays/         — Deterministic replay recordings

interface/
  visualization/   — Output / rendering layer
  controls/        — Player input handling

tests/             — Smoke tests for every core system
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

---

## Building the Smoke Tests

Requires a C++17-compatible compiler:

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
