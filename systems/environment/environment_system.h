#pragma once

// EnvironmentSystem (Engineering Schematic v0.1)
// Maintains spatial structure (WorldGrid) and applies resource diffusion.
// OMA Alignment: A2 (Core Systems — Support)

#include "world_grid.h"
#include "../../core/isystem.h"
#include "../../core/events/event_bus.h"
#include <cstddef>

class EnvironmentSystem : public ISystem {
public:
    EnvironmentSystem(EventBus& bus,
                      std::size_t gridWidth  = 10,
                      std::size_t gridHeight = 10);

    void init()     override;
    void update()   override;
    void shutdown() override;

    WorldGrid& grid();

private:
    EventBus&  bus_;
    WorldGrid  grid_;
    float      diffusionRate_;
};
