#include "simulation_controller.h"
#include "../tick_system/tick_manager.h"

SimulationController::SimulationController()
    : running_(false) {}

SimulationController::~SimulationController() {
    shutdown();
}

void SimulationController::init() {
    for (auto& system : systems_) {
        system->init();
    }
}

void SimulationController::run() {
    running_ = true;
    while (running_) {
        for (auto& system : systems_) {
            system->update();
        }
    }
}

void SimulationController::pause() {
    running_ = false;
}

void SimulationController::shutdown() {
    running_ = false;
    for (auto& system : systems_) {
        system->shutdown();
    }
    systems_.clear();
}

void SimulationController::registerSystem(std::shared_ptr<ISystem> system) {
    systems_.push_back(std::move(system));
}
