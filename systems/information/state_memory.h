#pragma once

// StateMemory — timestamped signal store with bounded history depth.
// OMA Alignment: A2 (Information System)

#include <string>
#include <deque>
#include <unordered_map>
#include <any>
#include <cstdint>
#include <cstddef>

struct MemoryEntry {
    uint64_t    tick;
    std::string key;
    std::any    value;
};

class StateMemory {
public:
    explicit StateMemory(std::size_t maxHistory = 100);

    void     record(uint64_t tick, const std::string& key, std::any value);
    std::any latest(const std::string& key) const;
    bool     has(const std::string& key) const;
    std::size_t entryCount() const;

private:
    std::size_t             maxHistory_;
    std::deque<MemoryEntry> entries_;
    std::unordered_map<std::string, std::size_t> latestIndex_;

    void rebuildIndex();
};
