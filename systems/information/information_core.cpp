#include "information_core.h"

void InformationSystem::init() {
    stateStore_.clear();
}

void InformationSystem::update() {
    // Propagate state changes and update memory patterns each tick.
    // TODO: implement pattern recognition and memory persistence.
}

void InformationSystem::shutdown() {
    stateStore_.clear();
}

void InformationSystem::setState(const std::string& key, std::any value) {
    stateStore_[key] = std::move(value);
}

std::any InformationSystem::getState(const std::string& key) const {
    auto it = stateStore_.find(key);
    if (it == stateStore_.end()) {
        return {};
    }
    return it->second;
}

bool InformationSystem::hasState(const std::string& key) const {
    return stateStore_.count(key) > 0;
}
