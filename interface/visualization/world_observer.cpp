#include "world_observer.h"
#include "../../core/influence/player_influence.h"
#include "../../systems/civilization/civilization_system.h"
#include <fstream>
#include <iostream>
#include <sstream>

WorldObserver::WorldObserver(EventBus& bus,
                             const EnergySystem& energySystem,
                             const EntitySystem& entitySystem,
                             const EnvironmentSystem& environmentSystem,
                             const CivilizationSystem* civilizationSystem,
                             std::size_t maxRecentEvents)
    : bus_(bus),
      energySystem_(energySystem),
      entitySystem_(entitySystem),
      environmentSystem_(environmentSystem),
      civilizationSystem_(civilizationSystem),
      maxRecentEvents_(maxRecentEvents),
      terminalEnabled_(false) {
    const EventType types[] = {
        EventType::EnergyTransfer,
        EventType::EntitySpawned,
        EventType::EntityDestroyed,
        EventType::EntityAction,
        EventType::InteractionResolved,
        EventType::StateCommitted,
        EventType::TickBegin,
        EventType::TickEnd,
        EventType::PlayerInfluenceApplied,
        EventType::WorldSnapshotCaptured,
        EventType::SettlementFounded,
        EventType::SettlementUpdated,
        EventType::CivilizationDecisionApplied,
        EventType::SeasonAdvanced,
        EventType::LandUseChanged,
        EventType::TechnologyAdvanced,
    };

    for (EventType type : types) {
        bus_.subscribe(type, [this](const SimEvent& event) { handleEvent(event); });
    }
}

void WorldObserver::setTerminalEnabled(bool enabled) {
    terminalEnabled_ = enabled;
}

void WorldObserver::saveJsonLines(const std::string& filePath) const {
    std::ofstream out(filePath);
    if (!out.is_open()) return;
    for (const auto& snapshot : snapshots_) {
        out << snapshot.toJson() << "\n";
    }
}

const std::vector<WorldSnapshot>& WorldObserver::snapshots() const {
    return snapshots_;
}

const WorldSnapshot* WorldObserver::latest() const {
    return snapshots_.empty() ? nullptr : &snapshots_.back();
}

void WorldObserver::handleEvent(const SimEvent& event) {
    rememberEvent(event);
    if (event.type == EventType::TickEnd) {
        capture(static_cast<uint64_t>(event.value));
        bus_.publish({EventType::WorldSnapshotCaptured, 0, 0, event.value, "snapshot"});
    }
}

void WorldObserver::capture(uint64_t tick) {
    WorldSnapshot snapshot;
    snapshot.tick = tick;
    snapshot.gridWidth = environmentSystem_.grid().width();
    snapshot.gridHeight = environmentSystem_.grid().height();
    snapshot.totalEnergy = energySystem_.totalEnergy();
    snapshot.totalResource = environmentSystem_.totalResource();
    snapshot.entityCount = entitySystem_.entityCount();
    snapshot.influenceMode = playerInfluenceModeName(environmentSystem_.influenceMode());

    for (std::size_t y = 0; y < snapshot.gridHeight; ++y) {
        for (std::size_t x = 0; x < snapshot.gridWidth; ++x) {
            snapshot.resources.push_back(environmentSystem_.grid().at(x, y).resource);
        }
    }

    for (const auto& record : entitySystem_.records()) {
        const auto& e = record.entity;
        snapshot.entities.push_back({e.id, e.x, e.y, e.energyReserve,
                                     entityStateName(e.state), e.tag,
                                     e.lastAction, e.memorySignal, e.age});
    }

    if (civilizationSystem_) {
        snapshot.season = civilizationSystem_->seasonName();
        snapshot.year = civilizationSystem_->year();
        snapshot.rainfallIndex = civilizationSystem_->rainfallIndex();
        snapshot.foodProductionIndex = civilizationSystem_->foodProductionIndex();
        snapshot.averageSoilFertility = civilizationSystem_->averageSoilFertility();
        snapshot.averageForestCover = civilizationSystem_->averageForestCover();
        snapshot.averageDesertification = civilizationSystem_->averageDesertification();
        snapshot.civicDecision = civilizationSystem_->decisionName();

        for (const auto& settlement : civilizationSystem_->settlements()) {
            snapshot.settlements.push_back({settlement.id,
                                            settlement.name,
                                            settlement.x,
                                            settlement.y,
                                            settlement.population,
                                            settlement.food,
                                            settlement.water,
                                            settlement.morale,
                                            settlement.health,
                                            settlement.technology,
                                            settlement.lastDecision,
                                            settlement.pressure});
        }
    }

    for (const auto& eventText : recentEvents_) {
        snapshot.recentEvents.push_back(eventText);
    }

    snapshots_.push_back(snapshot);

    if (terminalEnabled_) {
        std::cout << terminalObserver_.render(snapshot);
    }
}

void WorldObserver::rememberEvent(const SimEvent& event) {
    recentEvents_.push_back(describeEvent(event));
    while (recentEvents_.size() > maxRecentEvents_) {
        recentEvents_.pop_front();
    }
}

std::string WorldObserver::describeEvent(const SimEvent& event) const {
    std::ostringstream out;
    switch (event.type) {
        case EventType::EnergyTransfer:              out << "energy"; break;
        case EventType::EntitySpawned:               out << "spawn"; break;
        case EventType::EntityDestroyed:             out << "destroy"; break;
        case EventType::EntityAction:                out << "action"; break;
        case EventType::InteractionResolved:         out << "interaction"; break;
        case EventType::StateCommitted:              out << "state"; break;
        case EventType::TickBegin:                   out << "tick_begin"; break;
        case EventType::TickEnd:                     out << "tick_end"; break;
        case EventType::PlayerInfluenceApplied:      out << "influence"; break;
        case EventType::WorldSnapshotCaptured:       out << "snapshot"; break;
        case EventType::SettlementFounded:           out << "settlement_founded"; break;
        case EventType::SettlementUpdated:           out << "settlement"; break;
        case EventType::CivilizationDecisionApplied: out << "decision"; break;
        case EventType::SeasonAdvanced:              out << "season"; break;
        case EventType::LandUseChanged:              out << "land"; break;
        case EventType::TechnologyAdvanced:          out << "technology"; break;
        default:                                     out << "event"; break;
    }
    out << ":" << event.sourceId << "->" << event.targetId
        << " value=" << event.value;
    if (!event.tag.empty()) out << " tag=" << event.tag;
    return out.str();
}

std::string WorldObserver::entityStateName(EntityState state) const {
    switch (state) {
        case EntityState::Idle:   return "idle";
        case EntityState::Acting: return "acting";
        case EntityState::Dead:   return "dead";
        default:                  return "unknown";
    }
}
