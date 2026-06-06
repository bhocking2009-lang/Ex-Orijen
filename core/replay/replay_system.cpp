#include "replay_system.h"
#include <fstream>
#include <algorithm>

void ReplaySystem::recordEvent(uint64_t tick, const SimEvent& event) {
    getOrCreateRecord(tick).events.push_back(event);
}

void ReplaySystem::save(const std::string& filePath) const {
    std::ofstream out(filePath);
    if (!out.is_open()) return;
    for (const auto& record : records_) {
        out << "tick " << record.tick
            << " events " << record.events.size() << "\n";
        for (const auto& ev : record.events) {
            out << "  " << static_cast<int>(ev.type)
                << " " << ev.sourceId
                << " " << ev.targetId
                << " " << ev.value
                << " " << ev.tag << "\n";
        }
    }
}

bool ReplaySystem::load(const std::string& /*filePath*/) {
    // TODO: implement replay loading
    return false;
}

const std::vector<TickRecord>& ReplaySystem::records() const {
    return records_;
}

TickRecord& ReplaySystem::getOrCreateRecord(uint64_t tick) {
    // Linear search — acceptable for v0.1 where replay recordings are short.
    // A future optimisation can use an ordered map or sorted vector.
    for (auto& r : records_) {
        if (r.tick == tick) return r;
    }
    records_.push_back({tick, {}});
    return records_.back();
}
