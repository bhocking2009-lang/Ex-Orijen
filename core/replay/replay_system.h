#pragma once

// ReplaySystem - records per-tick events for deterministic replay and inspection.
// OMA Alignment: A5 (Data Layer - Replay)

#include "../events/event_types.h"
#include <vector>
#include <string>
#include <cstdint>
#include <utility>

struct TickRecord {
    uint64_t              tick;
    std::vector<SimEvent> events;
};

struct ReplaySummary {
    std::size_t tickCount = 0;
    std::size_t eventCount = 0;
    uint64_t firstTick = 0;
    uint64_t lastTick = 0;
    std::vector<std::pair<std::string, std::size_t>> eventsByType;

    std::string toString() const;
};

class ReplaySystem {
public:
    void recordEvent(uint64_t tick, const SimEvent& event);

    void save(const std::string& filePath) const;
    bool load(const std::string& filePath);

    ReplaySummary summary() const;
    std::string inspectTick(uint64_t tick) const;

    const std::vector<TickRecord>& records() const;

    static std::string eventTypeName(EventType type);

private:
    TickRecord& getOrCreateRecord(uint64_t tick);

    std::vector<TickRecord> records_;
};
