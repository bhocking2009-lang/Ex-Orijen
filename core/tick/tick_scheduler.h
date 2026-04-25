#pragma once

// TickScheduler — fixed-timestep tick loop with deterministic phase order.
// OMA Alignment: A3 (Simulation Control / Tick Manager)

#include <cstdint>
#include <functional>
#include <vector>

class TickScheduler {
public:
    explicit TickScheduler(double tickRateHz = 20.0);

    void setTickRate(double hz);

    // Register a callback invoked once per tick with the current tick index.
    void addTickCallback(std::function<void(uint64_t)> callback);

    // Run for exactly maxTicks ticks; maxTicks == 0 means run until stop().
    void run(uint64_t maxTicks = 0);

    void stop();

    uint64_t tickCount()  const;
    double   tickRateHz() const;

private:
    double   tickRateHz_;
    uint64_t tickCount_;
    bool     running_;
    std::vector<std::function<void(uint64_t)>> callbacks_;
};
