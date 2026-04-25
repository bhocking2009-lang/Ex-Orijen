#include "interaction_system.h"

InteractionSystem::InteractionSystem(EventBus& bus)
    : bus_(bus) {}

void InteractionSystem::init() {
    pending_.clear();
    resolvedLog_.clear();
}

void InteractionSystem::update() {
    resolveAll();
}

void InteractionSystem::shutdown() {
    pending_.clear();
    resolvedLog_.clear();
}

void InteractionSystem::queue(InteractionEvent event) {
    pending_.push_back(std::move(event));
}

const std::vector<InteractionEvent>& InteractionSystem::resolvedLog() const {
    return resolvedLog_;
}

void InteractionSystem::resolveAll() {
    for (auto& event : pending_) {
        event.resolved = true;
        bus_.publish({EventType::InteractionResolved,
                      event.sourceId, event.targetId,
                      event.energyTransferred, event.type});
        resolvedLog_.push_back(event);
    }
    pending_.clear();
}
