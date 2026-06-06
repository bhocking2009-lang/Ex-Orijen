#include "entity_core.h"

#include <algorithm>

void EntitySystem::init() {
    entities_.clear();
    nextId_ = 1;
}

void EntitySystem::update() {
    for (auto& entity : entities_) {
        if (!entity.active) continue;
        perceive(entity);
        decide(entity);
        act(entity);
    }
    // Remove inactive entities.
    entities_.erase(
        std::remove_if(entities_.begin(), entities_.end(),
                       [](const Entity& e) { return !e.active; }),
        entities_.end());
}

void EntitySystem::shutdown() {
    entities_.clear();
}

uint64_t EntitySystem::spawnEntity(double initialEnergy) {
    Entity e{nextId_++, initialEnergy, true};
    entities_.push_back(e);
    return e.id;
}

void EntitySystem::destroyEntity(uint64_t id) {
    for (auto& e : entities_) {
        if (e.id == id) {
            e.active = false;
        }
    }
}

const std::vector<Entity>& EntitySystem::entities() const {
    return entities_;
}

void EntitySystem::perceive(Entity& /*entity*/) {
    // TODO: query InformationSystem for local state.
}

void EntitySystem::decide(Entity& /*entity*/) {
    // TODO: apply behaviour rules loaded from data/configs.
}

void EntitySystem::act(Entity& entity) {
    // All actions carry an energy cost enforced here.
    const double actionCost = 0.01;
    if (entity.energyReserve >= actionCost) {
        entity.energyReserve -= actionCost;
    } else {
        entity.active = false;
    }
}
