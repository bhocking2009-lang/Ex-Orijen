#pragma once

// BehaviorModel — data-driven entity decision rules.
// OMA Alignment: A2 (Entity System)
// Constraint: behaviour thresholds and costs must be configurable.

#include "entity.h"

struct BehaviorResult {
    bool        acted;
    double      energyCost;
    std::string actionTag;
};

class BehaviorModel {
public:
    BehaviorResult evaluate(const Entity& entity) const;
};
