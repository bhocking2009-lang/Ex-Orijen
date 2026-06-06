# Ex Origine

**Ex Origine** is a modular real-time simulation framework that models the emergence of complexity from first principles.

It is not simply a game - it is a system where physics, information, and decision-making co-evolve, allowing players to guide, observe, or influence the unfolding of structured reality from its most primitive states.

> *"What happens when a world is allowed to become?"*

---

## Project Status

`v0.6 - Observable simulation loop established`

v0.1 closed the core runtime loop. v0.2 through v0.6 add the first visible world layer: tick-end world snapshots, contextual entity behavior, replay inspection, player influence modes, and a terminal observer grid.

---

## What Exists Now

- **v0.2 Observable snapshots**: `WorldObserver` captures one `WorldSnapshot` per tick and writes JSON-lines output.
- **v0.3 Entity behavior**: entities sense local grid resources, seek richer neighboring cells, harvest resources, observe patterns, and retain a simple memory signal.
- **v0.4 Replay inspection**: `ReplaySystem` records runtime events, reloads saved replay files, summarizes event counts, and inspects individual ticks.
- **v0.5 Player influence**: observer, energize, dampen, attract, stabilize, and architect modes nudge the environment through explicit events.
- **v0.6 Terminal observer**: `--observe` renders a simple resource grid with entity overlays as the simulation runs.

---

## Quick Start

Requires a C++17-compatible compiler. On Windows, the PowerShell verifier uses `g++` when available and otherwise falls back to Visual Studio `cl.exe` through `VsDevCmd.bat`.

Linux/macOS/Git Bash:

```sh
./scripts/verify_v0_6.sh
```

Windows PowerShell:

```powershell
.\scripts\verify_v0_6.ps1
```

Run the app directly after building through the verifier:

```powershell
.\build\v0_6_verify\ex_origine.exe --ticks 100 --mode attract --observe
```

Useful flags:

- `--ticks <n>`: number of ticks to run.
- `--mode <observer|energize|dampen|attract|stabilize|architect>`: player influence mode.
- `--observe`: print the terminal observer grid each tick.
- `--snapshot <path>`: write JSON-lines world snapshots.
- `--replay <path>`: write replay event records.
- `--config <path>`: use an alternate config file.

Default outputs:

- `runtime/replays/latest_world.jsonl`
- `runtime/replays/latest_events.replay`

---

## Repository Layout

```
docs/
  vision/          - Project vision and narrative
  architecture/    - Wireframes and structural specs
  systems/         - Per-system design documents
  engineering/     - Engineering implementation schematics
  release/         - Version closeout checklists and release gates

core/
  app/             - Application entry and CLI wiring
  config/          - ConfigLoader
  events/          - EventBus and event types
  influence/       - Player influence modes
  logging/         - SimulationLogger
  registry/        - SystemRegistry
  replay/          - ReplaySystem and replay inspection
  simulation/      - Integrated SimulationController
  state/           - SimulationState and StateManager
  tick/            - TickScheduler

systems/
  energy/          - EnergyPool and EnergySystem
  information/     - StateMemory and InformationSystem
  entity/          - Entity, BehaviorModel, EntitySystem
  environment/     - WorldGrid and EnvironmentSystem
  interaction/     - InteractionEvent and InteractionSystem

interface/
  visualization/   - WorldSnapshot, WorldObserver, TerminalObserver
  controls/        - Future player input handling

data/
  configs/         - simulation_config.json, system_defaults.json
  schemas/         - JSON schemas for data files
  rules/           - Future data-driven rule files
  seeds/           - Initial world conditions

runtime/
  logs/            - Simulation logs
  saves/           - Serialized simulation snapshots
  replays/         - Replay and world snapshot outputs

tests/
  smoke_tests.cpp
  unit/core_unit_tests.cpp
  integration/mvb_integration_test.cpp
  integration/v0_6_observer_integration_test.cpp
```

---

## Design Pillars

- **Everything is derived** - no arbitrary constants; all values emerge from rules.
- **Everything interacts** - systems communicate through defined interfaces and events.
- **Everything persists** - state changes are logged, replayed, or snapshotted.
- **Everything evolves** - systems are versioned and replaceable without rewrites.

---

## OMA Layer Alignment

| Layer | Responsibility |
|---|---|
| A1 | Foundational intent (vision documents) |
| A2 | Core systems: Energy, Information, Entity, Environment |
| A3 | Simulation control: Scheduler, events, replay |
| A4 | Player interface: Observer / Influencer / Architect |
| A5 | Data layer: Config, snapshots, replay, saves |
