#pragma once

// SimulationController — OMA Layer A3
// Owns the top-level simulation lifecycle: initialise, run, pause, and shut down.
// All subsystems must be registered here before the first tick is processed.

#include <vector>
#include <memory>

class ISystem;

class SimulationController {
public:
    SimulationController();
    ~SimulationController();

    // Lifecycle
    void init();
    void run();
    void pause();
    void shutdown();

    // System registration
    void registerSystem(std::shared_ptr<ISystem> system);

private:
    bool running_;
    std::vector<std::shared_ptr<ISystem>> systems_;
};
