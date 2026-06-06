#pragma once

// EventBus — decoupled publish/subscribe routing between simulation systems.
// OMA Alignment: A3 (Simulation Control)
// Constraint: systems communicate through events; no direct private state mutation.

#include "event_types.h"
#include <functional>
#include <unordered_map>
#include <vector>

using EventHandler = std::function<void(const SimEvent&)>;

class EventBus {
public:
    void subscribe(EventType type, EventHandler handler);
    void publish(const SimEvent& event);

private:
    std::unordered_map<uint8_t, std::vector<EventHandler>> handlers_;
};
