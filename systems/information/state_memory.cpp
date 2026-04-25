#include "state_memory.h"

StateMemory::StateMemory(std::size_t maxHistory)
    : maxHistory_(maxHistory) {}

void StateMemory::record(uint64_t tick, const std::string& key, std::any value) {
    entries_.push_back({tick, key, std::move(value)});
    latestIndex_[key] = entries_.size() - 1;
    if (entries_.size() > maxHistory_) {
        entries_.pop_front();
        // Full index rebuild after eviction — O(n) per eviction event.
        // Acceptable for the bounded history sizes used in v0.1.
        // A future optimisation can use a ring buffer with stable indices.
        rebuildIndex();
    }
}

std::any StateMemory::latest(const std::string& key) const {
    auto it = latestIndex_.find(key);
    if (it == latestIndex_.end()) return {};
    return entries_[it->second].value;
}

bool StateMemory::has(const std::string& key) const {
    return latestIndex_.count(key) > 0;
}

std::size_t StateMemory::entryCount() const {
    return entries_.size();
}

void StateMemory::rebuildIndex() {
    latestIndex_.clear();
    for (std::size_t i = 0; i < entries_.size(); ++i) {
        latestIndex_[entries_[i].key] = i;
    }
}
