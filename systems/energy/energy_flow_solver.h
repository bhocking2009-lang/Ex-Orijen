#pragma once

// EnergyFlowSolver — OMA Layer A2
// Resolves energy transfers across a node graph each tick.

#include <vector>

class EnergyFlowSolver {
public:
    explicit EnergyFlowSolver(double flowCoefficient = 0.5);

    // Mutates the energy levels of each node in-place.
    void solve(std::vector<double>& nodes);

private:
    double flowCoefficient_;
};
