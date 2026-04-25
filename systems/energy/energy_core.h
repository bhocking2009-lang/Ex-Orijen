#pragma once

// EnergySystem — OMA Layer A2
// Energy is the universal currency of all simulation processes.
// No entity may act and no transformation may occur without an energy cost.

#include "../../core/simulation_controller/isystem.h"

class EnergySystem : public ISystem {
public:
    EnergySystem();

    void init()     override;
    void update()   override;
    void shutdown() override;

    // Player capability: inject energy into the simulation.
    void injectEnergy(double amount);

    // Query total conserved energy.
    double totalEnergy() const;

private:
    void applyFlow();
    void applyDecay();

    double totalEnergy_;
};
