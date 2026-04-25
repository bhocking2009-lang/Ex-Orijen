#include "energy_system.h"

EnergySystem::EnergySystem(EventBus& bus, double decayRate)
    : bus_(bus), decayRate_(decayRate), nextPoolId_(1) {}

void EnergySystem::init() {
    pools_.clear();
    nextPoolId_ = 1;
}

void EnergySystem::update() {
    for (auto& pool : pools_) {
        pool->decay(decayRate_);
    }
}

void EnergySystem::shutdown() {
    pools_.clear();
}

uint64_t EnergySystem::createPool(double capacity, double initialLevel) {
    uint64_t id = nextPoolId_++;
    pools_.push_back(std::make_unique<EnergyPool>(id, capacity, initialLevel));
    return id;
}

EnergyPool* EnergySystem::getPool(uint64_t id) {
    for (auto& p : pools_) {
        if (p->id() == id) return p.get();
    }
    return nullptr;
}

bool EnergySystem::deposit(uint64_t poolId, double amount) {
    auto* pool = getPool(poolId);
    return pool && pool->deposit(amount);
}

bool EnergySystem::withdraw(uint64_t poolId, double amount) {
    auto* pool = getPool(poolId);
    if (!pool) return false;
    bool ok = pool->withdraw(amount);
    if (ok) {
        bus_.publish({EventType::EnergyTransfer, poolId, 0, amount, "withdraw"});
    }
    return ok;
}

double EnergySystem::totalEnergy() const {
    double total = 0.0;
    for (const auto& p : pools_) total += p->level();
    return total;
}
