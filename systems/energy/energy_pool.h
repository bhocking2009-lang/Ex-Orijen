#pragma once

// EnergyPool — a bounded energy reservoir.
// OMA Alignment: A2 (Energy System)
// Constraint: no pool may hold energy above its capacity or below zero.

#include <cstdint>

class EnergyPool {
public:
    EnergyPool(uint64_t id, double capacity, double initialLevel = 0.0);

    // Returns false if amount is negative or would exceed capacity.
    bool deposit(double amount);

    // Returns false if amount is negative or exceeds current level.
    bool withdraw(double amount);

    // Reduce level by (level * rate), clamped to [0, capacity].
    void decay(double rate);

    uint64_t id()       const;
    double   level()    const;
    double   capacity() const;

private:
    uint64_t id_;
    double   level_;
    double   capacity_;
};
