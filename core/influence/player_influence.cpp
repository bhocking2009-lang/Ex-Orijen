#include "player_influence.h"
#include <algorithm>

namespace {
std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}
}

PlayerInfluenceMode parsePlayerInfluenceMode(const std::string& value) {
    const std::string mode = lower(value);
    if (mode == "energize" || mode == "influencer") return PlayerInfluenceMode::Energize;
    if (mode == "dampen" || mode == "suppress") return PlayerInfluenceMode::Dampen;
    if (mode == "attract" || mode == "guide") return PlayerInfluenceMode::Attract;
    if (mode == "stabilize" || mode == "stabilise") return PlayerInfluenceMode::Stabilize;
    if (mode == "architect") return PlayerInfluenceMode::Architect;
    return PlayerInfluenceMode::Observer;
}

std::string playerInfluenceModeName(PlayerInfluenceMode mode) {
    switch (mode) {
        case PlayerInfluenceMode::Energize:  return "energize";
        case PlayerInfluenceMode::Dampen:    return "dampen";
        case PlayerInfluenceMode::Attract:   return "attract";
        case PlayerInfluenceMode::Stabilize: return "stabilize";
        case PlayerInfluenceMode::Architect: return "architect";
        case PlayerInfluenceMode::Observer:
        default:                             return "observer";
    }
}
