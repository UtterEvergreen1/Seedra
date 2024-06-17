#pragma once

#include "lce/processor.hpp"


// TODO: can this not just be turned into an enum?
class Rarity {
    c_i8 weight = 0;

public:
    Rarity() = default;

    explicit Rarity(c_i8 weight) : weight(weight) {}
    ND __forceinline i8 getWeight() const { return weight; }
};

struct Rarities {
    static inline const Rarity NONE = Rarity(0);
    static inline const Rarity COMMON = Rarity(10);
    static inline const Rarity UNCOMMON = Rarity(5);
    static inline const Rarity RARE = Rarity(2);
    static inline const Rarity EPIC = Rarity(1);
};
