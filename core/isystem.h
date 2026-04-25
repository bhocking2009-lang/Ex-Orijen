#pragma once

// ISystem — canonical base interface for every simulation system.
// OMA Alignment: A2 (Core Systems)
// File: core/isystem.h
//
// All new systems should include this header directly.
// The legacy copy at core/simulation_controller/isystem.h is preserved
// for backward compatibility with the v0.1 skeleton.

class ISystem {
public:
    virtual ~ISystem() = default;

    virtual void init()     = 0;
    virtual void update()   = 0;
    virtual void shutdown() = 0;
};
