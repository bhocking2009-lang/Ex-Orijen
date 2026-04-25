#include "energy_flow_solver.h"

EnergyFlowSolver::EnergyFlowSolver(double flowCoefficient)
    : flowCoefficient_(flowCoefficient) {}

void EnergyFlowSolver::solve(std::vector<double>& nodes) {
    if (nodes.size() < 2) return;

    // Simple gradient flow: transfer a fraction of the energy difference
    // between adjacent nodes each tick.
    std::vector<double> deltas(nodes.size(), 0.0);
    for (std::size_t i = 0; i + 1 < nodes.size(); ++i) {
        double diff = nodes[i] - nodes[i + 1];
        double transfer = diff * flowCoefficient_;
        deltas[i]     -= transfer;
        deltas[i + 1] += transfer;
    }
    for (std::size_t i = 0; i < nodes.size(); ++i) {
        nodes[i] += deltas[i];
    }
}
