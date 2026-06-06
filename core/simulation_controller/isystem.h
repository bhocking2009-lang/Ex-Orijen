#pragma once

// ISystem — base interface every simulation system must implement.
// OMA Alignment: A2 (Core Systems)

class ISystem {
public:
    virtual ~ISystem() = default;

    virtual void init()     = 0;
    virtual void update()   = 0;
    virtual void shutdown() = 0;
};
