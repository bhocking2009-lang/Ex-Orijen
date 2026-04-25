#include "simulation_state.h"

void SimulationState::set(const std::string& key, std::any value) {
    values[key] = std::move(value);
}

std::any SimulationState::get(const std::string& key) const {
    auto it = values.find(key);
    return (it != values.end()) ? it->second : std::any{};
}

bool SimulationState::has(const std::string& key) const {
    return values.count(key) > 0;
}
