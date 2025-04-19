#pragma once
#include <cstdint>
#include <string>

/**
 * @class StringHash
 * @brief A utility class for generating hash values from strings. Mimics the behavior of Java's String.hashCode().
 */
class StringHash {
public:
    /**
     * @brief Computes a hash value for a given string using a simple hashing algorithm.
     *
     * The hash is calculated by iterating over each character in the string and
     * applying the formula: `hash = 31 * hash + c`, where `c` is the ASCII value
     * of the character.
     *
     * @param str The input string to hash.
     * @return The computed hash value as a 32-bit integer.
     */
    static int32_t hash(const std::string &str) {
        int32_t hash = 0;
        for (char c : str) {
            hash = 31 * hash + static_cast<int32_t>(c);
        }
        return hash;
    }
};