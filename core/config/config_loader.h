#pragma once

// ConfigLoader — loads flat top-level key:value pairs from a JSON file.
// No external dependencies; designed for data-first configuration per OMA A5.

#include <string>
#include <unordered_map>
#include <cstdint>

class ConfigLoader {
public:
    // Returns true if the file was opened and parsed successfully.
    bool load(const std::string& filePath);

    std::string getString(const std::string& key, const std::string& defaultVal = "") const;
    double      getDouble(const std::string& key, double  defaultVal = 0.0)           const;
    int64_t     getInt   (const std::string& key, int64_t defaultVal = 0)             const;
    bool        getBool  (const std::string& key, bool    defaultVal = false)          const;
    bool        has      (const std::string& key)                                      const;

private:
    std::unordered_map<std::string, std::string> values_;

    static std::string trim(const std::string& s);
    static std::string stripQuotes(const std::string& s);
};
