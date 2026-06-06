#pragma once

// SystemRegistry — registers, orders, and provides access to simulation systems.
// OMA Alignment: A3 (Simulation Control)
// Constraint: systems may only be enabled/disabled through this registry.

#include "../isystem.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <utility>
#include <cstddef>

class SystemRegistry {
public:
    // Register a system under a unique name (first registration wins).
    void registerSystem(const std::string& name, std::shared_ptr<ISystem> system);

    // Nullify a system entry without collapsing the ordered list.
    void deregisterSystem(const std::string& name);

    bool                     hasSystem(const std::string& name) const;
    std::shared_ptr<ISystem> getSystem(const std::string& name) const;

    // Returns systems in registration order, suitable for update iteration.
    const std::vector<std::pair<std::string, std::shared_ptr<ISystem>>>& orderedSystems() const;

private:
    std::unordered_map<std::string, std::size_t>                         index_;
    std::vector<std::pair<std::string, std::shared_ptr<ISystem>>>        systems_;
};
