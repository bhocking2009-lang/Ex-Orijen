#pragma once

// EnvironmentSystem (Engineering Schematic v0.5)
// Maintains spatial structure (WorldGrid), applies player influence, and diffuses resources.
// OMA Alignment: A2 (Core Systems - Support)

#include "world_grid.h"
#include "../../core/isystem.h"
#include "../../core/events/event_bus.h"
#include "../../core/influence/player_influence.h"
#include <cstddef>

class EnvironmentSystem : public ISystem {
public:
    EnvironmentSystem(EventBus& bus,
                      std::size_t gridWidth  = 10,
                      std::size_t gridHeight = 10);

    void init()     override;
    void update()   override;
    void shutdown() override;

    void setInfluenceMode(PlayerInfluenceMode mode);
    PlayerInfluenceMode influenceMode() const;
    float totalResource() const;

    WorldGrid& grid();
    const WorldGrid& grid() const;

private:
    EventBus&  bus_;
    WorldGrid  grid_;
    float      diffusionRate_;
    PlayerInfluenceMode influenceMode_;

    void applyInfluence();
};
