#include "terminal_observer.h"
#include <sstream>
#include <vector>

std::string TerminalObserver::render(const WorldSnapshot& snapshot) const {
    std::vector<std::string> rows(snapshot.gridHeight, std::string(snapshot.gridWidth, '.'));

    for (std::size_t y = 0; y < snapshot.gridHeight; ++y) {
        for (std::size_t x = 0; x < snapshot.gridWidth; ++x) {
            rows[y][x] = resourceGlyph(snapshot.resourceAt(x, y));
        }
    }

    for (const auto& entity : snapshot.entities) {
        if (entity.x < snapshot.gridWidth && entity.y < snapshot.gridHeight) {
            rows[entity.y][entity.x] = 'E';
        }
    }

    for (const auto& settlement : snapshot.settlements) {
        if (settlement.x < snapshot.gridWidth && settlement.y < snapshot.gridHeight) {
            rows[settlement.y][settlement.x] = 'S';
        }
    }

    std::ostringstream out;
    out << "[OBSERVE] tick=" << snapshot.tick
        << " mode=" << snapshot.influenceMode
        << " energy=" << snapshot.totalEnergy
        << " resource=" << snapshot.totalResource
        << " entities=" << snapshot.entityCount;
    if (!snapshot.civicDecision.empty() || !snapshot.settlements.empty()) {
        out << " season=" << snapshot.season
            << " year=" << snapshot.year
            << " decision=" << snapshot.civicDecision
            << " rain=" << snapshot.rainfallIndex
            << " soil=" << snapshot.averageSoilFertility
            << " forest=" << snapshot.averageForestCover
            << " desert=" << snapshot.averageDesertification;
    }
    out << "\n";

    for (const auto& row : rows) {
        out << row << "\n";
    }

    if (!snapshot.settlements.empty()) {
        const auto& s = snapshot.settlements.front();
        out << "settlement#" << s.id
            << " name=" << s.name
            << " pop=" << s.population
            << " food=" << s.food
            << " water=" << s.water
            << " pressure=" << s.pressure
            << " decision=" << s.lastDecision << "\n";
    }

    if (!snapshot.entities.empty()) {
        const auto& e = snapshot.entities.front();
        out << "entity#" << e.id
            << " action=" << e.lastAction
            << " pos=" << e.x << "," << e.y
            << " memory=" << e.memorySignal << "\n";
    }

    return out.str();
}

char TerminalObserver::resourceGlyph(float value) const {
    if (value < 0.25f) return ' ';
    if (value < 0.75f) return '.';
    if (value < 1.50f) return ':';
    if (value < 2.50f) return '*';
    if (value < 4.00f) return 'o';
    return '@';
}
