#pragma once


#include "LegacyCubiomes/cubiomes/processor.hpp"

enum class Value {
    COMMON,
    UNCOMMON,
    RARE,
    EPIC,
    NONE
};

class Rarity {
private:
    const Value value = Value::NONE;
    const int weight = 0;
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
    static const Rarity EPIC;
};


