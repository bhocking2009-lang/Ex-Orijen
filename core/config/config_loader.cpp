#include "config_loader.h"
#include <fstream>
#include <algorithm>

bool ConfigLoader::load(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '{' || line[0] == '}') continue;

        auto colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;

        std::string key = stripQuotes(trim(line.substr(0, colonPos)));
        std::string val = trim(line.substr(colonPos + 1));
        if (!val.empty() && val.back() == ',') val.pop_back();
        val = stripQuotes(trim(val));

        if (!key.empty()) values_[key] = val;
    }
    return true;
}

std::string ConfigLoader::getString(const std::string& key, const std::string& defaultVal) const {
    auto it = values_.find(key);
    return (it != values_.end()) ? it->second : defaultVal;
}

double ConfigLoader::getDouble(const std::string& key, double defaultVal) const {
    auto it = values_.find(key);
    if (it == values_.end()) return defaultVal;
    try { return std::stod(it->second); }
    catch (const std::invalid_argument&) { return defaultVal; }
    catch (const std::out_of_range&)     { return defaultVal; }
}

int64_t ConfigLoader::getInt(const std::string& key, int64_t defaultVal) const {
    auto it = values_.find(key);
    if (it == values_.end()) return defaultVal;
    try { return std::stoll(it->second); }
    catch (const std::invalid_argument&) { return defaultVal; }
    catch (const std::out_of_range&)     { return defaultVal; }
}

bool ConfigLoader::getBool(const std::string& key, bool defaultVal) const {
    auto it = values_.find(key);
    if (it == values_.end()) return defaultVal;
    return it->second == "true";
}

bool ConfigLoader::has(const std::string& key) const {
    return values_.count(key) > 0;
}

std::string ConfigLoader::trim(const std::string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return {};
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::string ConfigLoader::stripQuotes(const std::string& s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
        return s.substr(1, s.size() - 2);
    return s;
}
