#pragma once

// SimulationLogger — records tick events, state diffs, and diagnostics.
// OMA Alignment: A5 (Data Layer)

#include <string>
#include <fstream>
#include <ostream>
#include <cstdint>

class SimulationLogger {
public:
    // If logPath is empty, output goes to stdout only.
    explicit SimulationLogger(const std::string& logPath = "");
    ~SimulationLogger();

    void logTickBegin(uint64_t tick);
    void logTickEnd(uint64_t tick);
    void log(const std::string& system, const std::string& message);
    void logError(const std::string& system, const std::string& message);

private:
    std::ofstream fileStream_;
    std::ostream* out_;
};
