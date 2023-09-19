#pragma once

#include "enchantment.hpp"

class EnchantmentData {
public:
    Enchantment* obj = nullptr;
    int level = 0;

    static std::string LEVEL_ROMAN[6];

    EnchantmentData() = default;
    EnchantmentData(Enchantment *enchantmentObj, int enchLevel) :
            obj(enchantmentObj), level(enchLevel) {}

    ND std::string toString() const {
        if (obj == nullptr)
            return "NULLPTR";
        if (level < 2)
            return obj->name;
        return obj->name + " " + LEVEL_ROMAN[level];
    }

    ND int getRarityWeight() const {
        return obj->rarity->getWeight();
    }

};


