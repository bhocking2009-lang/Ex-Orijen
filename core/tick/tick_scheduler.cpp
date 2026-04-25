#include "tick_scheduler.h"
#include <thread>
#include <chrono>

TickScheduler::TickScheduler(double tickRateHz)
    : tickRateHz_(tickRateHz), tickCount_(0), running_(false) {}

void TickScheduler::setTickRate(double hz) {
    tickRateHz_ = hz;
}

void TickScheduler::addTickCallback(std::function<void(uint64_t)> callback) {
    callbacks_.push_back(std::move(callback));
}

void TickScheduler::run(uint64_t maxTicks) {
    using namespace std::chrono;
    running_ = true;

    auto tickDuration = duration_cast<steady_clock::duration>(
        duration<double>(1.0 / tickRateHz_));
    auto nextTick = steady_clock::now();

    while (running_ && (maxTicks == 0 || tickCount_ < maxTicks)) {
        auto now = steady_clock::now();
        if (now < nextTick) {
            std::this_thread::sleep_until(nextTick);
        }
        for (auto& cb : callbacks_) cb(tickCount_);
        ++tickCount_;
        nextTick += tickDuration;
    }
    running_ = false;
}

void TickScheduler::stop() {
    running_ = false;
}

uint64_t TickScheduler::tickCount() const {
    return tickCount_;
}

double TickScheduler::tickRateHz() const {
    return tickRateHz_;
}
