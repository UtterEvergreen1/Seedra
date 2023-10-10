#pragma once

#include "enchantment.hpp"

class EnchantmentData {
public:
    Enchantment* obj = nullptr;
    int level = 0;

    static inline const std::string LEVEL_ROMAN[6] = {"", "I", "II", "III", "IV", "V"};

    EnchantmentData() = default;
    EnchantmentData(Enchantment *enchantmentObj, int enchLevel) :
            obj(enchantmentObj), level(enchLevel) {}

    ND std::string toString() const {
        if (obj == nullptr)
            return "NULLPTR";
        return obj->name + " " + LEVEL_ROMAN[level];
    }

    ND int getRarityWeight() const {
        return obj->rarity->getWeight();
    }

};

typedef std::vector<EnchantmentData> EnchDataVec_t;

