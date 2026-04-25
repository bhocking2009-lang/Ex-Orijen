#include "interaction_core.h"

void InteractionSystem::init() {
    pending_.clear();
    log_.clear();
}

void InteractionSystem::update() {
    resolveAll();
}

void InteractionSystem::shutdown() {
    pending_.clear();
    log_.clear();
}

void InteractionSystem::queueInteraction(InteractionEvent event) {
    pending_.push_back(std::move(event));
}

const std::vector<InteractionEvent>& InteractionSystem::log() const {
    return log_;
}

void InteractionSystem::resolveAll() {
    for (auto& event : pending_) {
        // TODO: apply interaction rules and modify system state.
        log_.push_back(event);
    }
    pending_.clear();
}
