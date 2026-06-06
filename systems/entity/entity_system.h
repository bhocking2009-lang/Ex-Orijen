#pragma once

// EntitySystem (Engineering Schematic v0.3)
// Manages agents: perception -> decision -> action with mandatory energy validation.
// OMA Alignment: A2 (Core Systems)
// Constraint: no entity may act without a successful energy withdrawal.

#include "entity.h"
#include "behavior_model.h"
#include "../../core/isystem.h"
#include "../../core/events/event_bus.h"
#include "../../core/influence/player_influence.h"
#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>

class EnergySystem;
class EnvironmentSystem;

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

    void attachEnvironment(EnvironmentSystem& environmentSystem);
    void setInfluenceMode(PlayerInfluenceMode mode);

    uint64_t    spawnEntity(double initialEnergy, uint64_t poolId,
                            const std::string& tag = "");
    void        destroyEntity(uint64_t id);
    std::size_t entityCount() const;
    const std::vector<EntityRecord>& records() const;

private:
    EventBus&     bus_;
    EnergySystem& energySystem_;
    EnvironmentSystem* environmentSystem_;
    PlayerInfluenceMode influenceMode_;
    BehaviorModel behaviorModel_;
    std::vector<EntityRecord> records_;
    uint64_t      nextId_;

    BehaviorContext buildContext(const Entity& entity) const;
    void applySuccessfulAction(EntityRecord& record, const BehaviorResult& result);
};
