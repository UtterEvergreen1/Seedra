#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentDurability : public Enchantment {
public:

    EnchantmentDurability(std::string name, const Rarity *rarity) :
        Enchantment(std::move(name), rarity, &Type::BREAKABLE, EnumName::DURABILITY, 3) {};

    int getMinEnchantability(int enchantmentLevel) override {
        return 5 + (enchantmentLevel - 1) * 8;
    }

    int getMaxEnchantability(int enchantmentLevel) override {
        return Enchantment::getMinEnchantability(enchantmentLevel) + 50;
    }

    ND bool canApply(const Items::Item *item) const override {
        return item->isDamageable() || Enchantment::canApply(item);
    }
};

