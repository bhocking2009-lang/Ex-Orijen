#pragma once

// WorldGrid — 2D spatial structure for the environment.
// OMA Alignment: A2 (Environment System)

#include <vector>
#include <cstdint>
#include <cstddef>

struct GridCell {
    float resource  = 1.0f;
    float energyCap = 50.0f;
};

class WorldGrid {
public:
    WorldGrid(std::size_t width, std::size_t height);

    GridCell&       at(std::size_t x, std::size_t y);
    const GridCell& at(std::size_t x, std::size_t y) const;

    std::size_t width()  const;
    std::size_t height() const;

    // Simple gradient diffusion of resource values across adjacent cells.
    void diffuse(float rate);

private:
    std::size_t           width_;
    std::size_t           height_;
    std::vector<GridCell> cells_;
};
