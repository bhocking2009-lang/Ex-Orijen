#pragma once

// InformationSystem — OMA Layer A2
// All simulation state lives here.
// Tracks memory and patterns that emerge from entity interactions.

#include "../../core/simulation_controller/isystem.h"
#include <string>
#include <unordered_map>
#include <any>

class InformationSystem : public ISystem {
public:
    void init()     override;
    void update()   override;
    void shutdown() override;

    // State accessors
    void  setState(const std::string& key, std::any value);
    std::any getState(const std::string& key) const;
    bool  hasState(const std::string& key) const;

private:
    std::unordered_map<std::string, std::any> stateStore_;
};
