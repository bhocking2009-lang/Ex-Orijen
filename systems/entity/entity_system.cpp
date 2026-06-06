#include "entity_system.h"
#include "../energy/energy_system.h"
#include <algorithm>

EntitySystem::EntitySystem(EventBus& bus, EnergySystem& energySystem)
    : bus_(bus), energySystem_(energySystem), nextId_(1) {}

void EntitySystem::init() {
    records_.clear();
    nextId_ = 1;
}

void EntitySystem::update() {
    for (auto& rec : records_) {
        if (!rec.entity.isAlive()) continue;

        BehaviorResult result = behaviorModel_.evaluate(rec.entity);
        if (!result.acted) continue;

        bool ok = energySystem_.withdraw(rec.poolId, result.energyCost);
        if (ok) {
            // Mirror the deduction locally so callers can read energyReserve
            // without querying the pool directly. The pool remains authoritative.
            rec.entity.energyReserve -= result.energyCost;
            bus_.publish({EventType::EntityAction, rec.entity.id, 0,
                          result.energyCost, result.actionTag});
        } else {
            rec.entity.state = EntityState::Dead;
            bus_.publish({EventType::EntityDestroyed, rec.entity.id, 0,
                          0.0, "energy_depleted"});
        }
    }

    // Remove dead entities at end of tick.
    records_.erase(
        std::remove_if(records_.begin(), records_.end(),
                       [](const EntityRecord& r) { return !r.entity.isAlive(); }),
        records_.end());
}

void EntitySystem::shutdown() {
    records_.clear();
}

uint64_t EntitySystem::spawnEntity(double initialEnergy, uint64_t poolId,
                                   const std::string& tag) {
    Entity e{nextId_++, initialEnergy, EntityState::Idle, tag};
    records_.push_back({e, poolId});
    bus_.publish({EventType::EntitySpawned, e.id, 0, initialEnergy, tag});
    return e.id;
}

void EntitySystem::destroyEntity(uint64_t id) {
    for (auto& rec : records_) {
        if (rec.entity.id == id) rec.entity.state = EntityState::Dead;
    }
}

std::size_t EntitySystem::entityCount() const {
    return records_.size();
}
