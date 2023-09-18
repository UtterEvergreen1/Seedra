#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentDigging : public Enchantment {
public:

    EnchantmentDigging(std::string name, const Rarity *rarity) :
        Enchantment(std::move(name), rarity, &Type::DIGGER, EnumName::DIGGING, 5) {};

    int getMinEnchantability(int enchantmentLevel) override {
        return 10 * enchantmentLevel - 9;
    }

    int getMaxEnchantability(int enchantmentLevel) override {
        return Enchantment::getMinEnchantability(enchantmentLevel) + 50;
    }

    ND bool canApply(const Items::Item *item) const override {
        return item->id == Items::SHEARS_ID || Enchantment::canApply(item);
    }
};

