#pragma once


#include "LegacyCubiomes/cubiomes/processor.hpp"

enum class Value {
    COMMON,
    UNCOMMON,
    RARE,
    VERY_RARE,
    NONE
};

class Rarity {
private:
    Value value;
    int weight;
public:
    Rarity() = default;

    Rarity(Value value, int weight) : value(value), weight(weight) {}
    ND Value getValue() const {
        return value;
    }
    ND int getWeight() const {
        return weight;
    }
};

struct Rarities {
    static const Rarity NONE;
    static const Rarity COMMON;
    static const Rarity UNCOMMON;
    static const Rarity RARE;
    static const Rarity VERY_RARE;
};


