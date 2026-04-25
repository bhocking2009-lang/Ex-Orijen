#pragma once

// Application — program entry wrapper.
// Parses CLI arguments, creates systems, drives SimulationController.
// OMA Alignment: A4 (Player / Interface Layer)

#include "../simulation/simulation_controller.h"
#include <memory>
#include <string>
#include <cstdint>

// Forward declarations to avoid pulling all system headers into this header.
class EnergySystem;
class InformationSystem;
class EntitySystem;
class EnvironmentSystem;
class InteractionSystem;

class Application {
public:
    Application();

    // Returns false if initialisation fails (e.g. bad config path).
    bool init(int argc, char* argv[]);

    void run();
    void shutdown();

private:
    SimulationController controller_;
    std::string          configPath_;
    uint64_t             maxTicks_;

    // Owned system instances (kept here for cross-system reference lifetimes).
    std::shared_ptr<EnergySystem>      energySys_;
    std::shared_ptr<InformationSystem> infoSys_;
    std::shared_ptr<EntitySystem>      entitySys_;
    std::shared_ptr<EnvironmentSystem> envSys_;
    std::shared_ptr<InteractionSystem> interactionSys_;
};
