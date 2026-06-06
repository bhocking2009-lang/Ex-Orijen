#pragma once

// InformationSystem (Engineering Schematic v0.1)
// Stores signals and maintains pattern history via StateMemory.
// OMA Alignment: A2 (Core Systems)

#include "state_memory.h"
#include "../../core/isystem.h"
#include "../../core/events/event_bus.h"
#include <string>
#include <any>
#include <cstdint>

class InformationSystem : public ISystem {
public:
    explicit InformationSystem(EventBus& bus);

    void init()     override;
    void update()   override;
    void shutdown() override;

    void     recordSignal(uint64_t tick, const std::string& key, std::any value);
    std::any latestSignal(const std::string& key) const;
    bool     hasSignal   (const std::string& key) const;

private:
    EventBus&   bus_;
    StateMemory memory_;
    uint64_t    currentTick_;
};
