#include "replay_system.h"
#include <algorithm>
#include <fstream>
#include <map>
#include <sstream>

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

bool ReplaySystem::load(const std::string& filePath) {
    std::ifstream in(filePath);
    if (!in.is_open()) return false;

    records_.clear();
    std::string line;
    TickRecord* current = nullptr;

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string marker;
        ss >> marker;

        if (marker == "tick") {
            uint64_t tick = 0;
            std::string eventsLabel;
            std::size_t eventCount = 0;
            ss >> tick >> eventsLabel >> eventCount;
            records_.push_back({tick, {}});
            current = &records_.back();
            continue;
        }

        if (!current) continue;

        int typeValue = 0;
        uint64_t source = 0;
        uint64_t target = 0;
        double value = 0.0;
        std::string tag;
        ss.clear();
        ss.str(line);
        ss >> typeValue >> source >> target >> value;
        std::getline(ss, tag);
        if (!tag.empty() && tag.front() == ' ') tag.erase(tag.begin());
        current->events.push_back({static_cast<EventType>(typeValue), source, target, value, tag});
    }

    return true;
}

ReplaySummary ReplaySystem::summary() const {
    ReplaySummary result;
    result.tickCount = records_.size();
    if (!records_.empty()) {
        result.firstTick = records_.front().tick;
        result.lastTick = records_.back().tick;
    }

    std::map<std::string, std::size_t> counts;
    for (const auto& record : records_) {
        result.eventCount += record.events.size();
        for (const auto& event : record.events) {
            counts[eventTypeName(event.type)] += 1;
        }
    }

    for (const auto& [type, count] : counts) {
        result.eventsByType.push_back({type, count});
    }
    return result;
}

std::string ReplaySystem::inspectTick(uint64_t tick) const {
    std::ostringstream out;
    for (const auto& record : records_) {
        if (record.tick != tick) continue;
        out << "tick " << record.tick << " events " << record.events.size() << "\n";
        for (const auto& event : record.events) {
            out << "  " << eventTypeName(event.type)
                << " source=" << event.sourceId
                << " target=" << event.targetId
                << " value=" << event.value
                << " tag=" << event.tag << "\n";
        }
        return out.str();
    }
    out << "tick " << tick << " not found\n";
    return out.str();
}

const std::vector<TickRecord>& ReplaySystem::records() const {
    return records_;
}

std::string ReplaySystem::eventTypeName(EventType type) {
    switch (type) {
        case EventType::EnergyTransfer:               return "EnergyTransfer";
        case EventType::EntitySpawned:                return "EntitySpawned";
        case EventType::EntityDestroyed:              return "EntityDestroyed";
        case EventType::EntityAction:                 return "EntityAction";
        case EventType::InteractionResolved:          return "InteractionResolved";
        case EventType::StateCommitted:               return "StateCommitted";
        case EventType::TickBegin:                    return "TickBegin";
        case EventType::TickEnd:                      return "TickEnd";
        case EventType::PlayerInfluenceApplied:       return "PlayerInfluenceApplied";
        case EventType::WorldSnapshotCaptured:        return "WorldSnapshotCaptured";
        case EventType::SettlementFounded:            return "SettlementFounded";
        case EventType::SettlementUpdated:            return "SettlementUpdated";
        case EventType::CivilizationDecisionApplied:  return "CivilizationDecisionApplied";
        case EventType::SeasonAdvanced:               return "SeasonAdvanced";
        case EventType::LandUseChanged:               return "LandUseChanged";
        case EventType::TechnologyAdvanced:           return "TechnologyAdvanced";
        default:                                      return "Unknown";
    }
}

TickRecord& ReplaySystem::getOrCreateRecord(uint64_t tick) {
    for (auto& r : records_) {
        if (r.tick == tick) return r;
    }
    records_.push_back({tick, {}});
    return records_.back();
}

std::string ReplaySummary::toString() const {
    std::ostringstream out;
    out << "ticks=" << tickCount
        << " events=" << eventCount
        << " first=" << firstTick
        << " last=" << lastTick;
    for (const auto& [type, count] : eventsByType) {
        out << "\n" << type << "=" << count;
    }
    return out.str();
}
