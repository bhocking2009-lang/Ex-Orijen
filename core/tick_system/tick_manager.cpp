#include "tick_manager.h"

#include <stdexcept>

TickManager::TickManager(double tickRateHz)
    : tickRateHz_(tickRateHz), tickCount_(0) {}

void TickManager::registerPhaseCallback(TickPhase phase, std::function<void()> callback) {
    auto idx = static_cast<uint8_t>(phase);
    phaseCallbacks_[idx] = std::move(callback);
}

void TickManager::tick() {
    for (uint8_t i = 0; i < static_cast<uint8_t>(TickPhase::COUNT); ++i) {
        if (phaseCallbacks_[i]) {
            phaseCallbacks_[i]();
        }
    }
    ++tickCount_;
}

uint64_t TickManager::tickCount() const {
    return tickCount_;
}
