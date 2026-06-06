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

struct WorldSnapshot {
    uint64_t tick = 0;
    std::size_t gridWidth = 0;
    std::size_t gridHeight = 0;
    double totalEnergy = 0.0;
    float totalResource = 0.0f;
    std::size_t entityCount = 0;
    std::string influenceMode;
    std::vector<float> resources;
    std::vector<EntitySnapshot> entities;
    std::vector<std::string> recentEvents;

    float resourceAt(std::size_t x, std::size_t y) const;
    std::string toJson() const;
};
