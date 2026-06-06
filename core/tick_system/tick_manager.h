#pragma once

// TickManager — OMA Layer A3
// Manages the discrete simulation tick loop.
// Enforces the canonical tick phase order defined in the wireframe.

#include <cstdint>
#include <functional>

enum class TickPhase : uint8_t {
    Input = 0,
    EnergyUpdate,
    InformationUpdate,
    EntityUpdate,
    InteractionResolution,
    EnvironmentUpdate,
    Persistence,
    COUNT
};

class TickManager {
public:
    explicit TickManager(double tickRateHz = 20.0);

    // Register a callback for a specific phase.
    void registerPhaseCallback(TickPhase phase, std::function<void()> callback);

    // Advance the simulation by one full tick (all phases in order).
    void tick();

    // Return the number of ticks elapsed since init.
    uint64_t tickCount() const;

private:
    double tickRateHz_;
    uint64_t tickCount_;
    std::function<void()> phaseCallbacks_[static_cast<uint8_t>(TickPhase::COUNT)];
};
