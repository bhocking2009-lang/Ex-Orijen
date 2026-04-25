#pragma once

// Entity — lightweight agent data.
// OMA Alignment: A2 (Entity System)

#include <cstdint>
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

    bool isAlive() const { return state != EntityState::Dead; }
};
