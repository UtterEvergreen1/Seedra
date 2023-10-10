#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentDurability : public Enchantment {
private:
    static constexpr inline int8_t LEVELS_MIN[3] = {};
    static constexpr inline int8_t LEVELS_MAX[3] = {};
public:

    EnchantmentDurability(std::string name, const Rarity *rarity) :
        Enchantment(std::move(name), rarity, &Type::BREAKABLE, EnumName::DURABILITY, 3) {};

    int getMinCost(int enchantmentLevel) override {
        return enchantmentLevel * 8 - 3;
    }

    int getMaxCost(int enchantmentLevel) override {
        return enchantmentLevel * 10 + 51;
    }

    ND bool canApply(const Items::Item *item) const override {
        return item->isDamageable() || Enchantment::canApply(item);
    }
};

