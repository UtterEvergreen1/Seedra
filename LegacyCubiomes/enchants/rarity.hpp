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
    static inline const Rarity NONE = Rarity(Value::NONE, 0);;
    static inline const Rarity COMMON = Rarity(Value::COMMON, 10);;
    static inline const Rarity UNCOMMON = Rarity(Value::UNCOMMON, 5);;
    static inline const Rarity RARE = Rarity(Value::RARE, 2);;
    static inline const Rarity EPIC = Rarity(Value::EPIC, 1);;
};



