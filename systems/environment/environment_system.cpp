#include "environment_system.h"
#include <algorithm>

EnvironmentSystem::EnvironmentSystem(EventBus& bus,
                                     std::size_t gridWidth,
                                     std::size_t gridHeight)
    : bus_(bus),
      grid_(gridWidth, gridHeight),
      diffusionRate_(0.05f),
      influenceMode_(PlayerInfluenceMode::Observer) {}

void EnvironmentSystem::init() {
    if (grid_.width() > 0 && grid_.height() > 0) {
        grid_.at(grid_.width() / 2, grid_.height() / 2).resource += 3.0f;
    }
}

void EnvironmentSystem::update() {
    applyInfluence();
    grid_.diffuse(diffusionRate_);
}

void EnvironmentSystem::shutdown() {}

void EnvironmentSystem::setInfluenceMode(PlayerInfluenceMode mode) {
    influenceMode_ = mode;
}

PlayerInfluenceMode EnvironmentSystem::influenceMode() const {
    return influenceMode_;
}

float EnvironmentSystem::totalResource() const {
    float total = 0.0f;
    for (std::size_t y = 0; y < grid_.height(); ++y) {
        for (std::size_t x = 0; x < grid_.width(); ++x) {
            total += grid_.at(x, y).resource;
        }
    }
    return total;
}

WorldGrid& EnvironmentSystem::grid() { return grid_; }
const WorldGrid& EnvironmentSystem::grid() const { return grid_; }

void EnvironmentSystem::applyInfluence() {
    if (grid_.width() == 0 || grid_.height() == 0) return;

    const std::size_t cx = grid_.width() / 2;
    const std::size_t cy = grid_.height() / 2;
    GridCell& center = grid_.at(cx, cy);

    switch (influenceMode_) {
        case PlayerInfluenceMode::Energize:
            center.resource += 0.25f;
            bus_.publish({EventType::PlayerInfluenceApplied, 0, 0, 0.25, "energize"});
            break;
        case PlayerInfluenceMode::Dampen:
            center.resource = std::max(0.0f, center.resource - 0.15f);
            bus_.publish({EventType::PlayerInfluenceApplied, 0, 0, -0.15, "dampen"});
            break;
        case PlayerInfluenceMode::Attract:
            center.resource += 0.15f;
            if (cx > 0) grid_.at(cx - 1, cy).resource += 0.05f;
            if (cx + 1 < grid_.width()) grid_.at(cx + 1, cy).resource += 0.05f;
            if (cy > 0) grid_.at(cx, cy - 1).resource += 0.05f;
            if (cy + 1 < grid_.height()) grid_.at(cx, cy + 1).resource += 0.05f;
            bus_.publish({EventType::PlayerInfluenceApplied, 0, 0, 0.35, "attract"});
            break;
        case PlayerInfluenceMode::Stabilize:
            for (std::size_t y = 0; y < grid_.height(); ++y) {
                for (std::size_t x = 0; x < grid_.width(); ++x) {
                    GridCell& cell = grid_.at(x, y);
                    if (cell.resource < 1.0f) cell.resource += 0.02f;
                    else if (cell.resource > 3.0f) cell.resource -= 0.02f;
                }
            }
            bus_.publish({EventType::PlayerInfluenceApplied, 0, 0, 0.0, "stabilize"});
            break;
        case PlayerInfluenceMode::Architect:
            for (std::size_t i = 0; i < std::min(grid_.width(), grid_.height()); ++i) {
                grid_.at(i, i).resource += 0.04f;
            }
            center.resource += 0.10f;
            bus_.publish({EventType::PlayerInfluenceApplied, 0, 0, 0.10, "architect"});
            break;
        case PlayerInfluenceMode::Observer:
        default:
            break;
    }
}
