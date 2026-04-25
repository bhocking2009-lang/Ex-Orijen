#include "environment_core.h"

void EnvironmentSystem::init() {
    regions_.clear();
    nextRegionId_ = 1;
}

void EnvironmentSystem::update() {
    redistributeResources();
}

void EnvironmentSystem::shutdown() {
    regions_.clear();
}

uint32_t EnvironmentSystem::addRegion(double resourceLevel, double energyCap) {
    Region r{nextRegionId_++, resourceLevel, energyCap};
    regions_.push_back(r);
    return r.id;
}

const std::vector<Region>& EnvironmentSystem::regions() const {
    return regions_;
}

void EnvironmentSystem::redistributeResources() {
    // TODO: apply spatial diffusion rules loaded from data/configs.
}
