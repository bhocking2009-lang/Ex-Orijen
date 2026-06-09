#include "civilization_system.h"
#include "../environment/environment_system.h"
#include "../environment/world_grid.h"
#include <algorithm>
#include <cmath>
#include <cctype>
#include <sstream>

namespace {
float clamp01(float value) {
    return std::max(0.0f, std::min(1.0f, value));
}

double clampDouble(double value, double lo, double hi) {
    return std::max(lo, std::min(hi, value));
}

std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}
}

CivilizationSystem::CivilizationSystem(EventBus& bus, EnvironmentSystem& environmentSystem)
    : bus_(bus),
      environmentSystem_(environmentSystem),
      decision_(CivicDecision::Stewardship),
      nextSettlementId_(1),
      tick_(0),
      rainfallIndex_(1.0f),
      foodProductionIndex_(1.0f) {}

void CivilizationSystem::init() {
    const std::size_t count = environmentSystem_.grid().width() * environmentSystem_.grid().height();
    land_.assign(count, LandUseCell{});
    settlements_.clear();
    nextSettlementId_ = 1;
    tick_ = 0;
    rainfallIndex_ = 1.0f;
    foodProductionIndex_ = 1.0f;
}

void CivilizationSystem::update() {
    ++tick_;
    applySeasonalClimate();

    for (auto& settlement : settlements_) {
        applyDecision(settlement);
    }

    updateLand();
    bus_.publish({EventType::LandUseChanged, 0, 0, averageDesertification(), decisionName()});

    for (auto& settlement : settlements_) {
        updateSettlement(settlement);
        publishSettlement(settlement);
    }

    syncLandToEnvironment();
    bus_.publish({EventType::SeasonAdvanced, 0, 0, rainfallIndex_, seasonName()});
}

void CivilizationSystem::shutdown() {
    settlements_.clear();
    land_.clear();
}

void CivilizationSystem::setDecision(CivicDecision decision) {
    decision_ = decision;
}

CivicDecision CivilizationSystem::decision() const {
    return decision_;
}

std::string CivilizationSystem::decisionName() const {
    return decisionName(decision_);
}

uint64_t CivilizationSystem::foundSettlement(const std::string& name,
                                             std::size_t x,
                                             std::size_t y,
                                             double population) {
    const std::size_t width = environmentSystem_.grid().width();
    const std::size_t height = environmentSystem_.grid().height();
    if (width == 0 || height == 0) return 0;

    Settlement settlement;
    settlement.id = nextSettlementId_++;
    settlement.name = name.empty() ? "First Hearth" : name;
    settlement.x = std::min(x, width - 1);
    settlement.y = std::min(y, height - 1);
    settlement.population = std::max(1.0, population);
    settlement.food = settlement.population * 1.2;
    settlement.water = settlement.population * 1.2;
    settlements_.push_back(settlement);
    bus_.publish({EventType::SettlementFounded, settlement.id, 0, settlement.population, settlement.name});
    return settlement.id;
}

const std::vector<Settlement>& CivilizationSystem::settlements() const {
    return settlements_;
}

const std::vector<LandUseCell>& CivilizationSystem::land() const {
    return land_;
}

std::string CivilizationSystem::seasonName() const {
    switch ((tick_ / 12) % 4) {
        case 0: return "spring";
        case 1: return "summer";
        case 2: return "autumn";
        default: return "winter";
    }
}

uint64_t CivilizationSystem::year() const {
    return tick_ / 48 + 1;
}

float CivilizationSystem::rainfallIndex() const {
    return rainfallIndex_;
}

float CivilizationSystem::foodProductionIndex() const {
    return foodProductionIndex_;
}

float CivilizationSystem::averageSoilFertility() const {
    if (land_.empty()) return 0.0f;
    float total = 0.0f;
    for (const auto& cell : land_) total += cell.soilFertility;
    return total / static_cast<float>(land_.size());
}

float CivilizationSystem::averageForestCover() const {
    if (land_.empty()) return 0.0f;
    float total = 0.0f;
    for (const auto& cell : land_) total += cell.forestCover;
    return total / static_cast<float>(land_.size());
}

float CivilizationSystem::averageDesertification() const {
    if (land_.empty()) return 0.0f;
    float total = 0.0f;
    for (const auto& cell : land_) total += cell.desertification;
    return total / static_cast<float>(land_.size());
}

CivicDecision CivilizationSystem::parseDecision(const std::string& value) {
    const std::string decision = lower(value);
    if (decision == "add_population" || decision == "population") return CivicDecision::AddPopulation;
    if (decision == "expand_farms" || decision == "farm") return CivicDecision::ExpandFarms;
    if (decision == "clear_forest" || decision == "deforest") return CivicDecision::ClearForest;
    if (decision == "rotate_crops" || decision == "rotation") return CivicDecision::RotateCrops;
    if (decision == "ration_food" || decision == "ration") return CivicDecision::RationFood;
    if (decision == "manage_water" || decision == "water") return CivicDecision::ManageWater;
    if (decision == "build_reservoir" || decision == "reservoir") return CivicDecision::BuildReservoir;
    if (decision == "reforest") return CivicDecision::Reforest;
    if (decision == "research_irrigation" || decision == "irrigation") return CivicDecision::ResearchIrrigation;
    if (decision == "research_sanitation" || decision == "sanitation") return CivicDecision::ResearchSanitation;
    if (decision == "research_crop_rotation" || decision == "crop_science") return CivicDecision::ResearchCropRotation;
    if (decision == "prepare_drought" || decision == "drought") return CivicDecision::PrepareForDrought;
    return CivicDecision::Stewardship;
}

std::string CivilizationSystem::decisionName(CivicDecision decision) {
    switch (decision) {
        case CivicDecision::AddPopulation:          return "add_population";
        case CivicDecision::ExpandFarms:            return "expand_farms";
        case CivicDecision::ClearForest:            return "clear_forest";
        case CivicDecision::RotateCrops:            return "rotate_crops";
        case CivicDecision::RationFood:             return "ration_food";
        case CivicDecision::ManageWater:            return "manage_water";
        case CivicDecision::BuildReservoir:         return "build_reservoir";
        case CivicDecision::Reforest:               return "reforest";
        case CivicDecision::ResearchIrrigation:     return "research_irrigation";
        case CivicDecision::ResearchSanitation:     return "research_sanitation";
        case CivicDecision::ResearchCropRotation:   return "research_crop_rotation";
        case CivicDecision::PrepareForDrought:      return "prepare_drought";
        case CivicDecision::Stewardship:
        default:                                    return "stewardship";
    }
}

LandUseCell& CivilizationSystem::landAt(std::size_t x, std::size_t y) {
    return land_[y * environmentSystem_.grid().width() + x];
}

const LandUseCell& CivilizationSystem::landAt(std::size_t x, std::size_t y) const {
    return land_[y * environmentSystem_.grid().width() + x];
}

void CivilizationSystem::applySeasonalClimate() {
    const std::string season = seasonName();
    rainfallIndex_ = 1.0f;
    if (season == "spring") rainfallIndex_ = 1.25f;
    else if (season == "summer") rainfallIndex_ = 0.70f;
    else if (season == "autumn") rainfallIndex_ = 1.10f;
    else rainfallIndex_ = 0.85f;

    if ((year() % 5 == 0) && season == "summer") rainfallIndex_ *= 0.55f;
    if ((year() % 7 == 0) && season == "spring") rainfallIndex_ *= 1.45f;
}

void CivilizationSystem::applyDecision(Settlement& settlement) {
    LandUseCell& local = landAt(settlement.x, settlement.y);
    settlement.lastDecision = decisionName(decision_);

    switch (decision_) {
        case CivicDecision::AddPopulation:
            settlement.population += 8.0;
            settlement.food -= 6.0;
            settlement.water -= 6.0;
            break;
        case CivicDecision::ExpandFarms:
            local.cropIntensity = clamp01(local.cropIntensity + 0.12f);
            local.forestCover = clamp01(local.forestCover - 0.04f);
            local.soilFertility = clamp01(local.soilFertility - 0.02f);
            break;
        case CivicDecision::ClearForest:
            settlement.food += 8.0;
            local.forestCover = clamp01(local.forestCover - 0.18f);
            local.floodRisk = clamp01(local.floodRisk + 0.08f);
            local.desertification = clamp01(local.desertification + 0.05f);
            break;
        case CivicDecision::RotateCrops:
            local.soilFertility = clamp01(local.soilFertility + 0.06f + static_cast<float>(settlement.technology) * 0.01f);
            local.cropIntensity = clamp01(local.cropIntensity - 0.03f);
            break;
        case CivicDecision::RationFood:
            settlement.food += settlement.population * 0.04;
            settlement.morale = clampDouble(settlement.morale - 0.03, 0.0, 1.0);
            break;
        case CivicDecision::ManageWater:
            settlement.water += 8.0 + settlement.technology * 2.0;
            local.floodRisk = clamp01(local.floodRisk - 0.04f);
            break;
        case CivicDecision::BuildReservoir:
            settlement.water += 16.0 + settlement.technology * 4.0;
            local.waterTable = clamp01(local.waterTable + 0.08f);
            local.floodRisk = clamp01(local.floodRisk - 0.06f);
            settlement.food -= 4.0;
            break;
        case CivicDecision::Reforest:
            local.forestCover = clamp01(local.forestCover + 0.10f);
            local.desertification = clamp01(local.desertification - 0.04f);
            local.floodRisk = clamp01(local.floodRisk - 0.03f);
            settlement.food -= 2.0;
            break;
        case CivicDecision::ResearchIrrigation:
            settlement.technology += 0.04;
            settlement.water += 3.0;
            break;
        case CivicDecision::ResearchSanitation:
            settlement.technology += 0.03;
            settlement.health = clampDouble(settlement.health + 0.04, 0.0, 1.0);
            settlement.water -= 1.0;
            break;
        case CivicDecision::ResearchCropRotation:
            settlement.technology += 0.035;
            local.soilFertility = clamp01(local.soilFertility + 0.03f);
            break;
        case CivicDecision::PrepareForDrought:
            settlement.water += 5.0;
            settlement.food += 4.0;
            settlement.morale = clampDouble(settlement.morale + 0.01, 0.0, 1.0);
            break;
        case CivicDecision::Stewardship:
        default:
            local.soilFertility = clamp01(local.soilFertility + 0.01f);
            local.forestCover = clamp01(local.forestCover + 0.01f);
            break;
    }

    bus_.publish({EventType::CivilizationDecisionApplied, settlement.id, 0, settlement.population, settlement.lastDecision});
}

void CivilizationSystem::updateLand() {
    foodProductionIndex_ = 0.0f;
    for (auto& cell : land_) {
        cell.rainfall = rainfallIndex_ * (0.65f + cell.forestCover * 0.35f);
        cell.waterTable = clamp01(cell.waterTable + (cell.rainfall - 1.0f) * 0.04f - cell.cropIntensity * 0.01f);
        cell.floodRisk = clamp01(cell.floodRisk + std::max(0.0f, cell.rainfall - 1.20f) * 0.06f - cell.forestCover * 0.01f);
        cell.soilFertility = clamp01(cell.soilFertility - cell.cropIntensity * 0.012f + cell.forestCover * 0.004f);
        cell.desertification = clamp01(cell.desertification + (1.0f - cell.waterTable) * 0.015f + (1.0f - cell.forestCover) * 0.006f - cell.rainfall * 0.004f);

        if (cell.desertification > 0.65f) cell.biome = "desert";
        else if (cell.forestCover > 0.65f) cell.biome = "forest";
        else if (cell.waterTable > 0.75f && cell.floodRisk > 0.35f) cell.biome = "wetland";
        else cell.biome = "grassland";

        foodProductionIndex_ += cell.cropIntensity * cell.soilFertility * cell.waterTable * (1.0f - cell.desertification);
    }
    if (!land_.empty()) foodProductionIndex_ /= static_cast<float>(land_.size());
}

void CivilizationSystem::updateSettlement(Settlement& settlement) {
    LandUseCell& local = landAt(settlement.x, settlement.y);
    const double rainfallBenefit = static_cast<double>(std::max(0.2f, local.rainfall));
    const double technologyBenefit = 1.0 + settlement.technology * 0.25;
    const double producedFood = settlement.population * 0.10 * local.cropIntensity * local.soilFertility * local.waterTable * rainfallBenefit * technologyBenefit;
    const double producedWater = settlement.population * 0.09 * local.waterTable * rainfallBenefit * (1.0 + settlement.technology * 0.15);
    const double foodNeed = settlement.population * 0.08;
    const double waterNeed = settlement.population * 0.07;

    settlement.food += producedFood - foodNeed;
    settlement.water += producedWater - waterNeed;

    const bool hungry = settlement.food < settlement.population * 0.25;
    const bool thirsty = settlement.water < settlement.population * 0.25;
    const bool flooded = local.floodRisk > 0.70f && rainfallIndex_ > 1.20f;
    const bool drought = rainfallIndex_ < 0.55f || local.waterTable < 0.25f;

    if (hungry || thirsty || flooded || drought) {
        settlement.morale = clampDouble(settlement.morale - 0.04, 0.0, 1.0);
        settlement.health = clampDouble(settlement.health - (thirsty ? 0.04 : 0.02), 0.0, 1.0);
    } else {
        settlement.morale = clampDouble(settlement.morale + 0.015, 0.0, 1.0);
        settlement.health = clampDouble(settlement.health + 0.01 + settlement.technology * 0.002, 0.0, 1.0);
        settlement.population += std::max(0.0, settlement.population * 0.003 * settlement.morale);
    }

    if (settlement.food < 0.0) {
        settlement.population = std::max(1.0, settlement.population + settlement.food * 0.10);
        settlement.food = 0.0;
    }
    if (settlement.water < 0.0) {
        settlement.population = std::max(1.0, settlement.population + settlement.water * 0.12);
        settlement.water = 0.0;
    }

    settlement.pressure = "stable";
    if (drought) settlement.pressure = "drought";
    if (flooded) settlement.pressure = "flood";
    if (hungry) settlement.pressure = "food_shortage";
    if (local.desertification > 0.65f) settlement.pressure = "desert_frontier";
}

void CivilizationSystem::syncLandToEnvironment() {
    WorldGrid& grid = environmentSystem_.grid();
    for (std::size_t y = 0; y < grid.height(); ++y) {
        for (std::size_t x = 0; x < grid.width(); ++x) {
            const LandUseCell& cell = landAt(x, y);
            GridCell& gridCell = grid.at(x, y);
            gridCell.resource = std::max(0.0f,
                (cell.soilFertility * 0.8f) +
                (cell.waterTable * 0.7f) +
                (cell.forestCover * 0.4f) -
                (cell.desertification * 0.9f));
        }
    }
}

void CivilizationSystem::publishSettlement(const Settlement& settlement) const {
    std::ostringstream tag;
    tag << settlement.name << ":" << settlement.pressure << ":" << settlement.lastDecision;
    bus_.publish({EventType::SettlementUpdated, settlement.id, 0, settlement.population, tag.str()});

    if (settlement.technology > 0.0) {
        bus_.publish({EventType::TechnologyAdvanced, settlement.id, 0, settlement.technology, settlement.name});
    }
}
