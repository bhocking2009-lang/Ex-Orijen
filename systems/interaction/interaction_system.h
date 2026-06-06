#pragma once

// InteractionSystem (Engineering Schematic v0.1)
// Queues and resolves cross-system interaction events each tick.
// OMA Alignment: A2 (Core Systems — Support)

#include "interaction_event.h"
#include "../../core/isystem.h"
#include "../../core/events/event_bus.h"
#include <vector>

class InteractionSystem : public ISystem {
public:
    explicit InteractionSystem(EventBus& bus);

    void init()     override;
    void update()   override;
    void shutdown() override;

    void queue(InteractionEvent event);
    const std::vector<InteractionEvent>& resolvedLog() const;

private:
    void resolveAll();

    EventBus&                     bus_;
    std::vector<InteractionEvent> pending_;
    std::vector<InteractionEvent> resolvedLog_;
};
