#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentDurability : public Enchantment {
public:

    EnchantmentDurability(std::string name, const Rarity *rarity) :
        Enchantment(std::move(name), rarity, &Type::BREAKABLE, EnumName::DURABILITY, 3) {};

    int getMinCost(int enchantmentLevel) override {
        return enchantmentLevel * 8 - 3;
    }

    int getMaxCost(int enchantmentLevel) override {
        return Enchantment::getMinCost(enchantmentLevel) + 50;
    }

    ND bool canApply(const Items::Item *item) const override {
        return item->isDamageable() || Enchantment::canApply(item);
    }
};

