#include "system_registry.h"

void SystemRegistry::registerSystem(const std::string& name, std::shared_ptr<ISystem> system) {
    if (index_.count(name)) return;
    index_[name] = systems_.size();
    systems_.emplace_back(name, std::move(system));
}

void SystemRegistry::deregisterSystem(const std::string& name) {
    auto it = index_.find(name);
    if (it != index_.end()) {
        systems_[it->second].second.reset();
    }
}

bool SystemRegistry::hasSystem(const std::string& name) const {
    auto it = index_.find(name);
    return it != index_.end() && systems_[it->second].second != nullptr;
}

std::shared_ptr<ISystem> SystemRegistry::getSystem(const std::string& name) const {
    auto it = index_.find(name);
    if (it == index_.end()) return nullptr;
    return systems_[it->second].second;
}

const std::vector<std::pair<std::string, std::shared_ptr<ISystem>>>&
SystemRegistry::orderedSystems() const {
    return systems_;
}
