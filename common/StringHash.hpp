#pragma once
#include <cstdint>
#include <string>

class StringHash {
public:
    static int32_t hash(const std::string &str) {
        int32_t hash = 0;
        for (char c : str) {
            hash = 31 * hash + static_cast<int32_t>(c);
        }
        return hash;
    }
};