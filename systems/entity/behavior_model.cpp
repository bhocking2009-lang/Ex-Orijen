#include "behavior_model.h"

BehaviorResult BehaviorModel::evaluate(const Entity& entity) const {
    // Rule: if the entity has enough energy, take a basic action at a fixed cost.
    // Thresholds and costs must be externalised to data files in a future iteration.
    const double threshold  = 0.1;
    const double actionCost = 0.01;

    if (!entity.isAlive() || entity.energyReserve < threshold) {
        return {false, 0.0, "none"};
    }
    return {true, actionCost, "basic_action"};
}
