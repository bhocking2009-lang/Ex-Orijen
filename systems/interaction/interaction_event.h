#pragma once

// InteractionEvent — data payload for a cross-system interaction.
// OMA Alignment: A2 (Interaction System)

#include <string>
#include <cstdint>

struct InteractionEvent {
    std::string type;
    uint64_t    sourceId          = 0;
    uint64_t    targetId          = 0;
    double      energyTransferred = 0.0;
    bool        resolved          = false;
};
