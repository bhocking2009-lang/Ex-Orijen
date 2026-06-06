#include "application.h"
#include "../../systems/energy/energy_system.h"
#include "../../systems/information/information_system.h"
#include "../../systems/entity/entity_system.h"
#include "../../systems/environment/environment_system.h"
#include "../../systems/interaction/interaction_system.h"
#include <iostream>

Application::Application()
    : snapshotPath_("runtime/replays/latest_world.jsonl"),
      replayPath_("runtime/replays/latest_events.replay"),
      maxTicks_(10),
      terminalObserverEnabled_(false),
      influenceMode_(PlayerInfluenceMode::Observer) {}

bool Application::init(int argc, char* argv[]) {
    configPath_ = "data/configs/simulation_config.json";
    bool ticksProvidedViaCli = false;
    bool snapshotProvidedViaCli = false;
    bool replayProvidedViaCli = false;
    bool observeProvidedViaCli = false;
    bool modeProvidedViaCli = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--config" && i + 1 < argc) {
            configPath_ = argv[++i];
        } else if (arg == "--ticks" && i + 1 < argc) {
            try {
                maxTicks_ = std::stoull(argv[++i]);
                ticksProvidedViaCli = true;
            } catch (...) {
                std::cerr << "[App] Invalid --ticks value; using default.\n";
            }
        } else if (arg == "--observe") {
            terminalObserverEnabled_ = true;
            observeProvidedViaCli = true;
        } else if (arg == "--snapshot" && i + 1 < argc) {
            snapshotPath_ = argv[++i];
            snapshotProvidedViaCli = true;
        } else if (arg == "--replay" && i + 1 < argc) {
            replayPath_ = argv[++i];
            replayProvidedViaCli = true;
        } else if ((arg == "--mode" || arg == "--influence") && i + 1 < argc) {
            influenceMode_ = parsePlayerInfluenceMode(argv[++i]);
            modeProvidedViaCli = true;
        }
    }

    if (!controller_.loadConfig(configPath_)) {
        std::cerr << "[App] Failed to load config: " << configPath_ << "\n";
        return false;
    }

    const auto& cfg = controller_.config();

    if (!ticksProvidedViaCli) {
        auto configTicks = cfg.getInt("max_ticks", static_cast<int64_t>(maxTicks_));
        if (configTicks > 0) {
            maxTicks_ = static_cast<uint64_t>(configTicks);
        } else {
            std::cerr << "[App] Ignoring non-positive max_ticks in config; using default.\n";
        }
    }

    if (!snapshotProvidedViaCli) {
        snapshotPath_ = cfg.getString("snapshot_path", snapshotPath_);
    }
    if (!replayProvidedViaCli) {
        replayPath_ = cfg.getString("replay_path", replayPath_);
    }
    if (!observeProvidedViaCli) {
        terminalObserverEnabled_ = cfg.getBool("observer_enabled", terminalObserverEnabled_);
    }
    if (!modeProvidedViaCli) {
        influenceMode_ = parsePlayerInfluenceMode(cfg.getString("player_mode", "observer"));
    }

    auto& bus = controller_.eventBus();

    energySys_     = std::make_shared<EnergySystem>(bus, cfg.getDouble("energy_decay_rate", 0.001));
    infoSys_       = std::make_shared<InformationSystem>(bus);
    entitySys_     = std::make_shared<EntitySystem>(bus, *energySys_);
    envSys_        = std::make_shared<EnvironmentSystem>(
                        bus,
                        static_cast<std::size_t>(cfg.getInt("grid_width",  10)),
                        static_cast<std::size_t>(cfg.getInt("grid_height", 10)));
    interactionSys_ = std::make_shared<InteractionSystem>(bus);

    envSys_->setInfluenceMode(influenceMode_);
    entitySys_->attachEnvironment(*envSys_);
    entitySys_->setInfluenceMode(influenceMode_);

    auto& reg = controller_.registry();
    reg.registerSystem("energy",      energySys_);
    reg.registerSystem("information", infoSys_);
    reg.registerSystem("entity",      entitySys_);
    reg.registerSystem("environment", envSys_);
    reg.registerSystem("interaction", interactionSys_);

    controller_.initSystems();

    observer_ = std::make_unique<WorldObserver>(bus, *energySys_, *entitySys_, *envSys_);
    observer_->setTerminalEnabled(terminalObserverEnabled_);

    double initialEnergy = cfg.getDouble("initial_energy", 100.0);
    uint64_t poolId = energySys_->createPool(initialEnergy, initialEnergy);
    int64_t initialEntities = cfg.getInt("initial_entities", 1);
    if (initialEntities < 1) initialEntities = 1;
    for (int64_t i = 0; i < initialEntities; ++i) {
        entitySys_->spawnEntity(initialEnergy * 0.1, poolId, "pioneer");
    }

    controller_.logger().log("App", "Simulation initialised - MVB ready.");
    controller_.logger().log("App", "Influence mode: " + playerInfluenceModeName(influenceMode_));
    return true;
}

void Application::run() {
    controller_.run(maxTicks_);
}

void Application::shutdown() {
    if (observer_) {
        observer_->saveJsonLines(snapshotPath_);
    }
    controller_.replay().save(replayPath_);
    controller_.shutdown();
    controller_.logger().log("App", "Simulation complete - clean exit.");
}
