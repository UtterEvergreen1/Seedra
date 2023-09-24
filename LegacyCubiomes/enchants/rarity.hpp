#pragma once


#include "LegacyCubiomes/cubiomes/processor.hpp"

class Rarity {
private:
    const int weight = 0;
public:
    Rarity() = default;

    Rarity(int weight) : weight(weight) {}
    ND inline int getWeight() const {
        return weight;
    }
};

struct Rarities {
    static inline const Rarity NONE = Rarity(0);
    static inline const Rarity COMMON = Rarity(10);
    static inline const Rarity UNCOMMON = Rarity(5);
    static inline const Rarity RARE = Rarity(2);
    static inline const Rarity EPIC = Rarity(1);
};

