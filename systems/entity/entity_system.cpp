#include "entity_system.h"
#include "../energy/energy_system.h"
#include "../environment/environment_system.h"
#include "../environment/world_grid.h"
#include <algorithm>
#include <sstream>

namespace {
std::size_t shiftedIndex(std::size_t value, int delta, std::size_t limit) {
    if (limit == 0) return 0;
    if (delta < 0) return value == 0 ? 0 : value - 1;
    if (delta > 0) return value + 1 >= limit ? limit - 1 : value + 1;
    return value;
}

double influenceBias(PlayerInfluenceMode mode) {
    switch (mode) {
        case PlayerInfluenceMode::Energize:  return 0.30;
        case PlayerInfluenceMode::Attract:   return 0.20;
        case PlayerInfluenceMode::Stabilize: return 0.10;
        case PlayerInfluenceMode::Architect: return 0.35;
        case PlayerInfluenceMode::Dampen:    return -0.20;
        case PlayerInfluenceMode::Observer:
        default:                             return 0.0;
    }
}
}

EntitySystem::EntitySystem(EventBus& bus, EnergySystem& energySystem)
    : bus_(bus),
      energySystem_(energySystem),
      environmentSystem_(nullptr),
      influenceMode_(PlayerInfluenceMode::Observer),
      nextId_(1) {}

void EntitySystem::init() {
    records_.clear();
    nextId_ = 1;
}

void EntitySystem::update() {
    for (auto& rec : records_) {
        if (!rec.entity.isAlive()) continue;

        rec.entity.age += 1;
        BehaviorResult result = behaviorModel_.evaluate(rec.entity, buildContext(rec.entity));
        if (!result.acted) {
            rec.entity.lastAction = result.actionTag;
            rec.entity.state = EntityState::Idle;
            continue;
        }

        bool ok = energySystem_.withdraw(rec.poolId, result.energyCost);
        if (ok) {
            applySuccessfulAction(rec, result);

            std::ostringstream tag;
            tag << result.actionTag << "@" << rec.entity.x << "," << rec.entity.y;
            bus_.publish({EventType::EntityAction, rec.entity.id, 0,
                          result.energyCost, tag.str()});
        } else {
            rec.entity.state = EntityState::Dead;
            rec.entity.lastAction = "energy_depleted";
            bus_.publish({EventType::EntityDestroyed, rec.entity.id, 0,
                          0.0, "energy_depleted"});
        }
    }

    // Remove dead entities at end of tick.
    records_.erase(
        std::remove_if(records_.begin(), records_.end(),
                       [](const EntityRecord& r) { return !r.entity.isAlive(); }),
        records_.end());
}

void EntitySystem::shutdown() {
    records_.clear();
}

void EntitySystem::attachEnvironment(EnvironmentSystem& environmentSystem) {
    environmentSystem_ = &environmentSystem;
}

void EntitySystem::setInfluenceMode(PlayerInfluenceMode mode) {
    influenceMode_ = mode;
}

uint64_t EntitySystem::spawnEntity(double initialEnergy, uint64_t poolId,
                                   const std::string& tag) {
    Entity e{nextId_++, initialEnergy, EntityState::Idle, tag};
    if (environmentSystem_ && environmentSystem_->grid().width() > 0 && environmentSystem_->grid().height() > 0) {
        e.x = environmentSystem_->grid().width() / 2;
        e.y = environmentSystem_->grid().height() / 2;
        if (!records_.empty()) {
            e.x = (e.x + records_.size()) % environmentSystem_->grid().width();
        }
    }
    records_.push_back({e, poolId});
    bus_.publish({EventType::EntitySpawned, e.id, 0, initialEnergy, tag});
    return e.id;
}

void EntitySystem::destroyEntity(uint64_t id) {
    for (auto& rec : records_) {
        if (rec.entity.id == id) rec.entity.state = EntityState::Dead;
    }
}

std::size_t EntitySystem::entityCount() const {
    return records_.size();
}

const std::vector<EntityRecord>& EntitySystem::records() const {
    return records_;
}

BehaviorContext EntitySystem::buildContext(const Entity& entity) const {
    BehaviorContext context;
    context.influenceBias = influenceBias(influenceMode_);

    if (!environmentSystem_) return context;

    WorldGrid& grid = environmentSystem_->grid();
    if (grid.width() == 0 || grid.height() == 0) return context;

    const std::size_t x = std::min(entity.x, grid.width() - 1);
    const std::size_t y = std::min(entity.y, grid.height() - 1);
    context.currentResource = grid.at(x, y).resource;
    context.bestNeighborResource = context.currentResource;

    const int directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    for (const auto& direction : directions) {
        std::size_t nx = shiftedIndex(x, direction[0], grid.width());
        std::size_t ny = shiftedIndex(y, direction[1], grid.height());
        float candidate = grid.at(nx, ny).resource;
        if (candidate > context.bestNeighborResource) {
            context.bestNeighborResource = candidate;
            context.bestDx = direction[0];
            context.bestDy = direction[1];
        }
    }

    return context;
}

void EntitySystem::applySuccessfulAction(EntityRecord& record, const BehaviorResult& result) {
    record.entity.energyReserve = std::max(0.0, record.entity.energyReserve - result.energyCost);
    record.entity.state = EntityState::Acting;
    record.entity.lastAction = result.actionTag;
    record.entity.memorySignal = (record.entity.memorySignal * 0.75) + (result.memorySignal * 0.25);

    if (environmentSystem_) {
        WorldGrid& grid = environmentSystem_->grid();
        if (grid.width() > 0 && grid.height() > 0) {
            record.entity.x = shiftedIndex(record.entity.x, result.dx, grid.width());
            record.entity.y = shiftedIndex(record.entity.y, result.dy, grid.height());

            if (result.actionTag == "harvest_resource") {
                GridCell& cell = grid.at(record.entity.x, record.entity.y);
                const float harvested = std::min(cell.resource, 0.05f);
                cell.resource -= harvested;
                if (harvested > 0.0f) {
                    const double gained = static_cast<double>(harvested);
                    energySystem_.deposit(record.poolId, gained);
                    record.entity.energyReserve += gained;
                }
            }
        }
    }
}
