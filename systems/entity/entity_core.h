#pragma once

// EntitySystem — OMA Layer A2
// Manages simulation agents: their perception, decision-making, and actions.
// No entity may act without paying an energy cost.

#include "../../core/simulation_controller/isystem.h"
#include <cstdint>
#include <vector>

struct Entity {
    uint64_t id;
    double   energyReserve;
    bool     active;
};

class EntitySystem : public ISystem {
public:
    void init()     override;
    void update()   override;
    void shutdown() override;

    uint64_t spawnEntity(double initialEnergy);
    void     destroyEntity(uint64_t id);

    const std::vector<Entity>& entities() const;

private:
    void perceive(Entity& entity);
    void decide(Entity& entity);
    void act(Entity& entity);

    std::vector<Entity> entities_;
    uint64_t            nextId_;
};
