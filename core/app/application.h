#pragma once

// Application - program entry wrapper.
// Parses CLI arguments, creates systems, drives SimulationController.
// OMA Alignment: A4 (Player / Interface Layer)

#include "../simulation/simulation_controller.h"
#include "../influence/player_influence.h"
#include "../../interface/visualization/world_observer.h"
#include "../../systems/civilization/civilization_system.h"
#include <memory>
#include <string>
#include <cstdint>

class EnergySystem;
class InformationSystem;
class EntitySystem;
class EnvironmentSystem;
class InteractionSystem;

class Application {
public:
    Application();

    bool init(int argc, char* argv[]);

    void run();
    void shutdown();

private:
    SimulationController controller_;
    std::string          configPath_;
    std::string          snapshotPath_;
    std::string          replayPath_;
    std::string          settlementName_;
    uint64_t             maxTicks_;
    double               initialPopulation_;
    bool                 terminalObserverEnabled_;
    PlayerInfluenceMode  influenceMode_;
    CivicDecision        civicDecision_;

    std::shared_ptr<EnergySystem>       energySys_;
    std::shared_ptr<InformationSystem>  infoSys_;
    std::shared_ptr<EntitySystem>       entitySys_;
    std::shared_ptr<EnvironmentSystem>  envSys_;
    std::shared_ptr<CivilizationSystem> civilizationSys_;
    std::shared_ptr<InteractionSystem>  interactionSys_;
    std::unique_ptr<WorldObserver>      observer_;
};
