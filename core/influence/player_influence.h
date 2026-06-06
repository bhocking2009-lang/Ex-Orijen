#pragma once

#include <cstdint>
#include <string>

// PlayerInfluenceMode maps the A4 player posture into low-authority runtime nudges.
enum class PlayerInfluenceMode : uint8_t {
    Observer,
    Energize,
    Dampen,
    Attract,
    Stabilize,
    Architect,
};

PlayerInfluenceMode parsePlayerInfluenceMode(const std::string& value);
std::string playerInfluenceModeName(PlayerInfluenceMode mode);
