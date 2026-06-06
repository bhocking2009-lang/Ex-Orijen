#include "world_grid.h"
#include <algorithm>
#include <cassert>

WorldGrid::WorldGrid(std::size_t width, std::size_t height)
    : width_(width), height_(height), cells_(width * height) {}

GridCell& WorldGrid::at(std::size_t x, std::size_t y) {
    assert(x < width_ && y < height_);
    return cells_[y * width_ + x];
}

const GridCell& WorldGrid::at(std::size_t x, std::size_t y) const {
    assert(x < width_ && y < height_);
    return cells_[y * width_ + x];
}

std::size_t WorldGrid::width()  const { return width_; }
std::size_t WorldGrid::height() const { return height_; }

void WorldGrid::diffuse(float rate) {
    std::vector<float> deltas(cells_.size(), 0.0f);
    for (std::size_t y = 0; y < height_; ++y) {
        for (std::size_t x = 0; x < width_; ++x) {
            float center = at(x, y).resource;
            if (x + 1 < width_) {
                float diff = (center - at(x + 1, y).resource) * rate;
                deltas[y * width_ + x]         -= diff;
                deltas[y * width_ + (x + 1)]   += diff;
            }
            if (y + 1 < height_) {
                float diff = (center - at(x, y + 1).resource) * rate;
                deltas[y       * width_ + x]   -= diff;
                deltas[(y + 1) * width_ + x]   += diff;
            }
        }
    }
    for (std::size_t i = 0; i < cells_.size(); ++i) {
        cells_[i].resource = std::max(0.0f, cells_[i].resource + deltas[i]);
    }
}
