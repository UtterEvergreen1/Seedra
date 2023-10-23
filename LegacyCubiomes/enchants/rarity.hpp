#pragma once

#include "LegacyCubiomes/utils/processor.hpp"


class Rarity {
private:
    const int8_t weight = 0;

public:
    Rarity() = default;

    explicit Rarity(int8_t weight) : weight(weight) {}
    ND __forceinline int8_t getWeight() const { return weight; }
};

struct Rarities {
    static inline const Rarity NONE = Rarity(0);
    static inline const Rarity COMMON = Rarity(10);
    static inline const Rarity UNCOMMON = Rarity(5);
    static inline const Rarity RARE = Rarity(2);
    static inline const Rarity EPIC = Rarity(1);
};
