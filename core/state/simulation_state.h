#pragma once

// SimulationState — snapshot of authoritative simulation values at a given tick.
// OMA Alignment: A5 (Data Layer)

#include <unordered_map>
#include <string>
#include <any>
#include <cstdint>

struct SimulationState {
    uint64_t tick = 0;
    std::unordered_map<std::string, std::any> values;

    void     set(const std::string& key, std::any value);
    std::any get(const std::string& key) const;
    bool     has(const std::string& key) const;
};
