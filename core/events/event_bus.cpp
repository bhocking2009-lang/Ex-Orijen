#include "event_bus.h"

void EventBus::subscribe(EventType type, EventHandler handler) {
    handlers_[static_cast<uint8_t>(type)].push_back(std::move(handler));
}

void EventBus::publish(const SimEvent& event) {
    auto it = handlers_.find(static_cast<uint8_t>(event.type));
    if (it == handlers_.end()) return;
    for (auto& h : it->second) h(event);
}
