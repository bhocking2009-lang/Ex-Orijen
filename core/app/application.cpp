#include "application.h"
#include "../../systems/energy/energy_system.h"
#include "../../systems/information/information_system.h"
#include "../../systems/entity/entity_system.h"
#include "../../systems/environment/environment_system.h"
#include "../../systems/interaction/interaction_system.h"
#include <iostream>

Application::Application()
    : maxTicks_(10) {}

bool Application::init(int argc, char* argv[]) {
    configPath_ = "data/configs/simulation_config.json";

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--config" && i + 1 < argc) {
            configPath_ = argv[++i];
        } else if (arg == "--ticks" && i + 1 < argc) {
            try { maxTicks_ = std::stoull(argv[++i]); } catch (...) {}
        }
    }

    controller_.loadConfig(configPath_);

    // Override maxTicks from config if not set via CLI.
    auto configTicks = controller_.config().getInt("max_ticks", static_cast<int64_t>(maxTicks_));
    maxTicks_ = static_cast<uint64_t>(configTicks);

    auto& bus = controller_.eventBus();
    const auto& cfg = controller_.config();

    // Create systems in dependency order (energy before entity).
    energySys_     = std::make_shared<EnergySystem>(bus, cfg.getDouble("energy_decay_rate", 0.001));
    infoSys_       = std::make_shared<InformationSystem>(bus);
    entitySys_     = std::make_shared<EntitySystem>(bus, *energySys_);
    envSys_        = std::make_shared<EnvironmentSystem>(
                        bus,
                        static_cast<std::size_t>(cfg.getInt("grid_width",  10)),
                        static_cast<std::size_t>(cfg.getInt("grid_height", 10)));
    interactionSys_ = std::make_shared<InteractionSystem>(bus);

    // Register in tick-update order (matches wireframe phases).
    auto& reg = controller_.registry();
    reg.registerSystem("energy",      energySys_);
    reg.registerSystem("information", infoSys_);
    reg.registerSystem("entity",      entitySys_);
    reg.registerSystem("environment", envSys_);
    reg.registerSystem("interaction", interactionSys_);

    controller_.initSystems();

    // Seed the world: one energy pool, one pioneer entity (MVB requirements).
    double initialEnergy = cfg.getDouble("initial_energy", 100.0);
    uint64_t poolId = energySys_->createPool(initialEnergy, initialEnergy);
    entitySys_->spawnEntity(initialEnergy * 0.1, poolId, "pioneer");

    controller_.logger().log("App", "Simulation initialised — MVB ready.");
    return true;
}

void Application::run() {
    controller_.run(maxTicks_);
}

void Application::shutdown() {
    controller_.shutdown();
    controller_.logger().log("App", "Simulation complete — clean exit.");
}
