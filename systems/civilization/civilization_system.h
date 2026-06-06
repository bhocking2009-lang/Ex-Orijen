#pragma once

// CivilizationSystem - player-facing settlement, ecology, season, and technology layer.
// OMA Alignment: A4 gameplay decisions expressed through A2/A5 simulation state.

#include "../../core/events/event_bus.h"
#include "../../core/isystem.h"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

class EnvironmentSystem;

enum class CivicDecision : uint8_t {
    Stewardship,
    AddPopulation,
    ExpandFarms,
    ClearForest,
    RotateCrops,
    RationFood,
    ManageWater,
    BuildReservoir,
    Reforest,
    ResearchIrrigation,
    ResearchSanitation,
    ResearchCropRotation,
    PrepareForDrought,
};

struct LandUseCell {
    float forestCover = 0.45f;
    float soilFertility = 1.0f;
    float waterTable = 1.0f;
    float cropIntensity = 0.2f;
    float desertification = 0.0f;
    float floodRisk = 0.1f;
    float rainfall = 1.0f;
    std::string biome = "grassland";
};

struct Settlement {
    uint64_t id = 0;
    std::string name;
    std::size_t x = 0;
    std::size_t y = 0;
    double population = 100.0;
    double food = 120.0;
    double water = 120.0;
    double morale = 0.75;
    double health = 0.80;
    double technology = 0.0;
    std::string lastDecision = "stewardship";
    std::string pressure = "stable";
};

class CivilizationSystem : public ISystem {
public:
    CivilizationSystem(EventBus& bus, EnvironmentSystem& environmentSystem);

    void init() override;
    void update() override;
    void shutdown() override;

    void setDecision(CivicDecision decision);
    CivicDecision decision() const;
    std::string decisionName() const;

    uint64_t foundSettlement(const std::string& name,
                             std::size_t x,
                             std::size_t y,
                             double population);

    const std::vector<Settlement>& settlements() const;
    const std::vector<LandUseCell>& land() const;

    std::string seasonName() const;
    uint64_t year() const;
    float rainfallIndex() const;
    float foodProductionIndex() const;
    float averageSoilFertility() const;
    float averageForestCover() const;
    float averageDesertification() const;

    static CivicDecision parseDecision(const std::string& value);
    static std::string decisionName(CivicDecision decision);

private:
    EventBus& bus_;
    EnvironmentSystem& environmentSystem_;
    CivicDecision decision_;
    std::vector<Settlement> settlements_;
    std::vector<LandUseCell> land_;
    uint64_t nextSettlementId_;
    uint64_t tick_;
    float rainfallIndex_;
    float foodProductionIndex_;

    LandUseCell& landAt(std::size_t x, std::size_t y);
    const LandUseCell& landAt(std::size_t x, std::size_t y) const;

    void applySeasonalClimate();
    void applyDecision(Settlement& settlement);
    void updateLand();
    void updateSettlement(Settlement& settlement);
    void syncLandToEnvironment();
    void publishSettlement(const Settlement& settlement) const;
};
