#pragma once

// EnergySystem (Engineering Schematic v0.1)
// Manages EnergyPools; applies decay each tick; validates energy costs.
// OMA Alignment: A2 (Core Systems)

#include "energy_pool.h"
#include "../../core/isystem.h"
#include "../../core/events/event_bus.h"
#include <vector>
#include <memory>
#include <cstdint>

class EnergySystem : public ISystem {
public:
    EnergySystem(EventBus& bus, double decayRate = 0.001);

    void init()     override;
    void update()   override;
    void shutdown() override;

    // Create a new pool; returns its unique ID.
    uint64_t    createPool(double capacity, double initialLevel = 0.0);
    EnergyPool* getPool(uint64_t id);

    // Returns false and publishes no event if the pool lacks sufficient energy.
    bool deposit (uint64_t poolId, double amount);
    bool withdraw(uint64_t poolId, double amount);

    double totalEnergy() const;

private:
    EventBus& bus_;
    double    decayRate_;
    uint64_t  nextPoolId_;
    std::vector<std::unique_ptr<EnergyPool>> pools_;
};
