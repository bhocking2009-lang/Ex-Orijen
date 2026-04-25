#include "state_manager.h"

void StateManager::commit(SimulationState state) {
    history_.push_back(current_);
    current_ = std::move(state);
}

const SimulationState& StateManager::current() const {
    return current_;
}

std::size_t StateManager::historySize() const {
    return history_.size();
}
