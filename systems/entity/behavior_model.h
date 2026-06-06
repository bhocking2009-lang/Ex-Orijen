#pragma once

// BehaviorModel - data-driven entity decision rules.
// OMA Alignment: A2 (Entity System)
// Constraint: behaviour thresholds and costs must remain explicit and inspectable.

#include "entity.h"

struct BehaviorContext {
    float currentResource = 1.0f;
    float bestNeighborResource = 1.0f;
    int bestDx = 0;
    int bestDy = 0;
    double influenceBias = 0.0;
};

struct BehaviorResult {
    bool        acted;
    double      energyCost;
    std::string actionTag;
    int         dx = 0;
    int         dy = 0;
    double      memorySignal = 0.0;
};

class BehaviorModel {
public:
    BehaviorModel(double minActionEnergy = 0.1,
                  double moveCost = 0.02,
                  double harvestCost = 0.01,
                  double observeCost = 0.005);

    BehaviorResult evaluate(const Entity& entity,
                            const BehaviorContext& context = {}) const;

private:
    double minActionEnergy_;
    double moveCost_;
    double harvestCost_;
    double observeCost_;
};
