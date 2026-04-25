#include "simulation_controller.h"

SimulationController::SimulationController()
    : tickScheduler_(20.0) {}

bool SimulationController::loadConfig(const std::string& configPath) {
    if (!configPath.empty() && !config_.load(configPath)) {
        return false;
    }
    double hz = config_.getDouble("tick_rate_hz", 20.0);
    tickScheduler_.setTickRate(hz);
    return true;
}

bool SimulationController::initSystems() {
    for (auto& [name, system] : registry_.orderedSystems()) {
        if (system) system->init();
    }
    tickScheduler_.addTickCallback([this](uint64_t tick) { onTick(tick); });
    return true;
}

void SimulationController::run(uint64_t maxTicks) {
    tickScheduler_.run(maxTicks);
}

void SimulationController::stop() {
    tickScheduler_.stop();
}

void SimulationController::shutdown() {
    tickScheduler_.stop();
    for (auto& [name, system] : registry_.orderedSystems()) {
        if (system) system->shutdown();
    }
}

SystemRegistry&     SimulationController::registry()     { return registry_; }
EventBus&           SimulationController::eventBus()     { return eventBus_; }
StateManager&       SimulationController::stateManager() { return stateManager_; }
SimulationLogger&   SimulationController::logger()       { return logger_; }
const ConfigLoader& SimulationController::config() const { return config_; }

void SimulationController::onTick(uint64_t tick) {
    eventBus_.publish({EventType::TickBegin, 0, 0, static_cast<double>(tick), {}});
    logger_.logTickBegin(tick);

    for (auto& [name, system] : registry_.orderedSystems()) {
        if (system) system->update();
    }

    SimulationState state;
    state.tick = tick;
    stateManager_.commit(std::move(state));

    logger_.logTickEnd(tick);
    eventBus_.publish({EventType::TickEnd, 0, 0, static_cast<double>(tick), {}});
}
