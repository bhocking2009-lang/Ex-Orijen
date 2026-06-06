#include "behavior_model.h"

BehaviorModel::BehaviorModel(double minActionEnergy,
                             double moveCost,
                             double harvestCost,
                             double observeCost)
    : minActionEnergy_(minActionEnergy),
      moveCost_(moveCost),
      harvestCost_(harvestCost),
      observeCost_(observeCost) {}

BehaviorResult BehaviorModel::evaluate(const Entity& entity,
                                       const BehaviorContext& context) const {
    if (!entity.isAlive() || entity.energyReserve < minActionEnergy_) {
        return {false, 0.0, "rest"};
    }

    const float seekMargin = 0.05f;
    if (context.bestNeighborResource > context.currentResource + seekMargin &&
        (context.bestDx != 0 || context.bestDy != 0)) {
        return {true, moveCost_, "seek_energy", context.bestDx, context.bestDy,
                context.bestNeighborResource};
    }

    if (context.currentResource > 0.5f || context.influenceBias > 0.0) {
        return {true, harvestCost_, "harvest_resource", 0, 0,
                context.currentResource + context.influenceBias};
    }

    return {true, observeCost_, "observe_pattern", 0, 0,
            context.currentResource};
}
