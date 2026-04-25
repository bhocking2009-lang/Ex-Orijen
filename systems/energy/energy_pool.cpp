#include "energy_pool.h"
#include <algorithm>

EnergyPool::EnergyPool(uint64_t id, double capacity, double initialLevel)
    : id_(id),
      capacity_(capacity),
      level_(std::min(std::max(initialLevel, 0.0), capacity)) {}

bool EnergyPool::deposit(double amount) {
    if (amount < 0.0) return false;
    level_ = std::min(level_ + amount, capacity_);
    return true;
}

bool EnergyPool::withdraw(double amount) {
    if (amount < 0.0 || amount > level_) return false;
    level_ -= amount;
    return true;
}

void EnergyPool::decay(double rate) {
    level_ = std::max(0.0, level_ - level_ * rate);
}

uint64_t EnergyPool::id()       const { return id_; }
double   EnergyPool::level()    const { return level_; }
double   EnergyPool::capacity() const { return capacity_; }
