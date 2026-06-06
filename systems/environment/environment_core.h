#pragma once

// EnvironmentSystem — OMA Layer A2 (Support)
// Defines the spatial structure of the simulation and manages resource distribution.
// Constrains how energy flows between regions.

#include "../../core/simulation_controller/isystem.h"
#include <cstdint>
#include <vector>

struct Region {
    uint32_t id;
    double   resourceLevel;
    double   energyCap;
};

class EnvironmentSystem : public ISystem {
public:
    void init()     override;
    void update()   override;
    void shutdown() override;

    uint32_t addRegion(double resourceLevel, double energyCap);
    const std::vector<Region>& regions() const;

private:
    void redistributeResources();

    std::vector<Region> regions_;
    uint32_t            nextRegionId_;
};
