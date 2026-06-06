#pragma once

// InteractionSystem — OMA Layer A2 (Support)
// Resolves cross-system effects: collisions, transformations, emergent reactions.
// Every interaction is logged for traceability.

#include "../../core/simulation_controller/isystem.h"
#include <cstdint>
#include <functional>
#include <vector>
#include <string>

struct InteractionEvent {
    std::string type;
    uint64_t    sourceId;
    uint64_t    targetId;
    double      energyTransferred;
};

class InteractionSystem : public ISystem {
public:
    void init()     override;
    void update()   override;
    void shutdown() override;

    void queueInteraction(InteractionEvent event);
    const std::vector<InteractionEvent>& log() const;

private:
    void resolveAll();

    std::vector<InteractionEvent> pending_;
    std::vector<InteractionEvent> log_;
};
