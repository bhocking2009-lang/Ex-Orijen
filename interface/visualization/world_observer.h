#pragma once

#include "world_snapshot.h"
#include "terminal_observer.h"
#include "../../core/events/event_bus.h"
#include "../../systems/energy/energy_system.h"
#include "../../systems/entity/entity_system.h"
#include "../../systems/environment/environment_system.h"
#include <cstddef>
#include <cstdint>
#include <deque>
#include <string>
#include <vector>

class CivilizationSystem;

class WorldObserver {
public:
    WorldObserver(EventBus& bus,
                  const EnergySystem& energySystem,
                  const EntitySystem& entitySystem,
                  const EnvironmentSystem& environmentSystem,
                  const CivilizationSystem* civilizationSystem = nullptr,
                  std::size_t maxRecentEvents = 8);

    void setTerminalEnabled(bool enabled);
    void saveJsonLines(const std::string& filePath) const;

    const std::vector<WorldSnapshot>& snapshots() const;
    const WorldSnapshot* latest() const;

private:
    EventBus& bus_;
    const EnergySystem& energySystem_;
    const EntitySystem& entitySystem_;
    const EnvironmentSystem& environmentSystem_;
    const CivilizationSystem* civilizationSystem_;
    std::size_t maxRecentEvents_;
    bool terminalEnabled_;
    TerminalObserver terminalObserver_;
    std::deque<std::string> recentEvents_;
    std::vector<WorldSnapshot> snapshots_;

    void handleEvent(const SimEvent& event);
    void capture(uint64_t tick);
    void rememberEvent(const SimEvent& event);
    std::string describeEvent(const SimEvent& event) const;
    std::string entityStateName(EntityState state) const;
};
