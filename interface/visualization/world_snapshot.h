#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

struct EntitySnapshot {
    uint64_t id = 0;
    std::size_t x = 0;
    std::size_t y = 0;
    double energyReserve = 0.0;
    std::string state;
    std::string tag;
    std::string lastAction;
    double memorySignal = 0.0;
    uint64_t age = 0;
};

struct SettlementSnapshot {
    uint64_t id = 0;
    std::string name;
    std::size_t x = 0;
    std::size_t y = 0;
    double population = 0.0;
    double food = 0.0;
    double water = 0.0;
    double morale = 0.0;
    double health = 0.0;
    double technology = 0.0;
    std::string lastDecision;
    std::string pressure;
};

struct WorldSnapshot {
    uint64_t tick = 0;
    std::size_t gridWidth = 0;
    std::size_t gridHeight = 0;
    double totalEnergy = 0.0;
    float totalResource = 0.0f;
    std::size_t entityCount = 0;
    std::string influenceMode;
    std::string season;
    uint64_t year = 1;
    float rainfallIndex = 1.0f;
    float foodProductionIndex = 1.0f;
    float averageSoilFertility = 0.0f;
    float averageForestCover = 0.0f;
    float averageDesertification = 0.0f;
    std::string civicDecision;
    std::vector<float> resources;
    std::vector<EntitySnapshot> entities;
    std::vector<SettlementSnapshot> settlements;
    std::vector<std::string> recentEvents;

    float resourceAt(std::size_t x, std::size_t y) const;
    std::string toJson() const;
};
