#include "energy_core.h"

EnergySystem::EnergySystem() : totalEnergy_(0.0) {}

void EnergySystem::init() {
    totalEnergy_ = 0.0;
}

void EnergySystem::update() {
    applyFlow();
    applyDecay();
}

void EnergySystem::shutdown() {
    totalEnergy_ = 0.0;
}

void EnergySystem::injectEnergy(double amount) {
    totalEnergy_ += amount;
}

double EnergySystem::totalEnergy() const {
    return totalEnergy_;
}

void EnergySystem::applyFlow() {
    // TODO: delegate to energy_flow_solver
}

void EnergySystem::applyDecay() {
    // TODO: load decay rate from energy_rules.json
}
