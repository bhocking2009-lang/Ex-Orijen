#pragma once

// ReplaySystem — records per-tick events for deterministic replay.
// OMA Alignment: A5 (Data Layer — Replay)

#include "../events/event_types.h"
#include <vector>
#include <string>
#include <cstdint>

struct TickRecord {
    uint64_t              tick;
    std::vector<SimEvent> events;
};

class ReplaySystem {
public:
    void recordEvent(uint64_t tick, const SimEvent& event);

    void save(const std::string& filePath) const;
    bool load(const std::string& filePath);

    const std::vector<TickRecord>& records() const;

private:
    TickRecord& getOrCreateRecord(uint64_t tick);

    std::vector<TickRecord> records_;
};
