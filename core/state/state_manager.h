#pragma once

// StateManager — holds the authoritative committed simulation state and its history.
// OMA Alignment: A5 (Data Layer)

#include "simulation_state.h"
#include <vector>
#include <cstddef>

class StateManager {
public:
    // Commit a new state snapshot; the previous current_ is pushed to history.
    void commit(SimulationState state);

    const SimulationState& current()     const;
    std::size_t            historySize() const;

private:
    SimulationState              current_;
    std::vector<SimulationState> history_;
};
