#pragma once

// Entity - lightweight agent data.
// OMA Alignment: A2 (Entity System)

#include <cstdint>
#include <cstddef>
#include <string>

enum class EntityState : uint8_t {
    Idle,
    Acting,
    Dead,
};

struct Entity {
    uint64_t    id;
    double      energyReserve;
    EntityState state;
    std::string tag;
    std::size_t x = 0;
    std::size_t y = 0;
    double      memorySignal = 0.0;
    std::string lastAction = "spawned";
    uint64_t    age = 0;

    bool isAlive() const { return state != EntityState::Dead; }
};
