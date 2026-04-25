#pragma once

// EventTypes — defines all simulation event categories and the SimEvent payload.
// OMA Alignment: A3 (Simulation Control) / A2 (Core Systems)

#include <string>
#include <cstdint>

enum class EventType : uint8_t {
    EnergyTransfer,
    EntitySpawned,
    EntityDestroyed,
    EntityAction,
    InteractionResolved,
    StateCommitted,
    TickBegin,
    TickEnd,
};

struct SimEvent {
    EventType   type;
    uint64_t    sourceId = 0;
    uint64_t    targetId = 0;
    double      value    = 0.0;
    std::string tag;
};
