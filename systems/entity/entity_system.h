#pragma once

// EntitySystem (Engineering Schematic v0.1)
// Manages agents: perception → decision → action with mandatory energy validation.
// OMA Alignment: A2 (Core Systems)
// Constraint: no entity may act without a successful energy withdrawal.

#include "entity.h"
#include "behavior_model.h"
#include "../../core/isystem.h"
#include "../../core/events/event_bus.h"
#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>

class EnergySystem;

// Bundles an Entity with the EnergyPool it draws from.
struct EntityRecord {
    Entity   entity;
    uint64_t poolId;
};

class EntitySystem : public ISystem {
public:
    EntitySystem(EventBus& bus, EnergySystem& energySystem);

    void init()     override;
    void update()   override;
    void shutdown() override;

    uint64_t    spawnEntity(double initialEnergy, uint64_t poolId,
                            const std::string& tag = "");
    void        destroyEntity(uint64_t id);
    std::size_t entityCount() const;

private:
    EventBus&     bus_;
    EnergySystem& energySystem_;
    BehaviorModel behaviorModel_;
    std::vector<EntityRecord> records_;
    uint64_t      nextId_;
};
