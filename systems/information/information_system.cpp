#include "information_system.h"

InformationSystem::InformationSystem(EventBus& bus)
    : bus_(bus), memory_(100), currentTick_(0) {}

void InformationSystem::init() {
    currentTick_ = 0;
}

void InformationSystem::update() {
    ++currentTick_;
}

void InformationSystem::shutdown() {
    currentTick_ = 0;
}

void InformationSystem::recordSignal(uint64_t tick, const std::string& key, std::any value) {
    memory_.record(tick, key, std::move(value));
}

std::any InformationSystem::latestSignal(const std::string& key) const {
    return memory_.latest(key);
}

bool InformationSystem::hasSignal(const std::string& key) const {
    return memory_.has(key);
}
