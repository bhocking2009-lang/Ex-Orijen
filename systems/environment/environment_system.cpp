#include "environment_system.h"

EnvironmentSystem::EnvironmentSystem(EventBus& bus,
                                     std::size_t gridWidth,
                                     std::size_t gridHeight)
    : bus_(bus), grid_(gridWidth, gridHeight), diffusionRate_(0.05f) {}

void EnvironmentSystem::init() {}

void EnvironmentSystem::update() {
    grid_.diffuse(diffusionRate_);
}

void EnvironmentSystem::shutdown() {}

WorldGrid& EnvironmentSystem::grid() { return grid_; }
