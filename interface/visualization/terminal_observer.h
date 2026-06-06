#pragma once

#include "world_snapshot.h"
#include <string>

class TerminalObserver {
public:
    std::string render(const WorldSnapshot& snapshot) const;

private:
    char resourceGlyph(float value) const;
};
