#include "simulation_logger.h"
#include <iostream>

SimulationLogger::SimulationLogger(const std::string& logPath)
    : out_(&std::cout) {
    if (!logPath.empty()) {
        fileStream_.open(logPath, std::ios::app);
        if (fileStream_.is_open()) out_ = &fileStream_;
    }
}

SimulationLogger::~SimulationLogger() {
    if (fileStream_.is_open()) fileStream_.close();
}

void SimulationLogger::logTickBegin(uint64_t tick) {
    *out_ << "[TICK " << tick << "] === BEGIN ===\n";
}

void SimulationLogger::logTickEnd(uint64_t tick) {
    *out_ << "[TICK " << tick << "] === END ===\n";
}

void SimulationLogger::log(const std::string& system, const std::string& message) {
    *out_ << "[" << system << "] " << message << "\n";
}

void SimulationLogger::logError(const std::string& system, const std::string& message) {
    *out_ << "[" << system << "][ERROR] " << message << "\n";
}
