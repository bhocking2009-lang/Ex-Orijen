#include "world_snapshot.h"
#include <iomanip>
#include <sstream>

namespace {
std::string escapeJson(const std::string& value) {
    std::ostringstream out;
    for (char c : value) {
        switch (c) {
            case '"': out << "\\\""; break;
            case '\\': out << "\\\\"; break;
            case '\n': out << "\\n"; break;
            case '\r': out << "\\r"; break;
            case '\t': out << "\\t"; break;
            default: out << c; break;
        }
    }
    return out.str();
}
}

float WorldSnapshot::resourceAt(std::size_t x, std::size_t y) const {
    if (x >= gridWidth || y >= gridHeight) return 0.0f;
    const std::size_t index = y * gridWidth + x;
    if (index >= resources.size()) return 0.0f;
    return resources[index];
}

std::string WorldSnapshot::toJson() const {
    std::ostringstream out;
    out << std::fixed << std::setprecision(4);
    out << "{";
    out << "\"tick\":" << tick << ",";
    out << "\"grid\":{" << "\"width\":" << gridWidth << ",\"height\":" << gridHeight << "},";
    out << "\"totalEnergy\":" << totalEnergy << ",";
    out << "\"totalResource\":" << totalResource << ",";
    out << "\"entityCount\":" << entityCount << ",";
    out << "\"influenceMode\":\"" << escapeJson(influenceMode) << "\",";

    out << "\"resources\":[";
    for (std::size_t i = 0; i < resources.size(); ++i) {
        if (i) out << ",";
        out << resources[i];
    }
    out << "],";

    out << "\"entities\":[";
    for (std::size_t i = 0; i < entities.size(); ++i) {
        const auto& e = entities[i];
        if (i) out << ",";
        out << "{";
        out << "\"id\":" << e.id << ",";
        out << "\"x\":" << e.x << ",";
        out << "\"y\":" << e.y << ",";
        out << "\"energyReserve\":" << e.energyReserve << ",";
        out << "\"state\":\"" << escapeJson(e.state) << "\",";
        out << "\"tag\":\"" << escapeJson(e.tag) << "\",";
        out << "\"lastAction\":\"" << escapeJson(e.lastAction) << "\",";
        out << "\"memorySignal\":" << e.memorySignal << ",";
        out << "\"age\":" << e.age;
        out << "}";
    }
    out << "],";

    out << "\"recentEvents\":[";
    for (std::size_t i = 0; i < recentEvents.size(); ++i) {
        if (i) out << ",";
        out << "\"" << escapeJson(recentEvents[i]) << "\"";
    }
    out << "]";
    out << "}";
    return out.str();
}
