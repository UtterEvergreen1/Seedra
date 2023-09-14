#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentThorns : public Enchantment {
public:

    EnchantmentThorns(std::string name, const Rarity *rarity) :
            Enchantment(std::move(name), rarity, &Type::ARMOR_CHEST, EnumName::THORNS, 3) {};

    int getMinEnchantability(int enchantmentLevel) override {
        return 10 + 20 * (enchantmentLevel - 1);
    }

    int getMaxEnchantability(int enchantmentLevel) override {
        return Enchantment::getMinEnchantability(enchantmentLevel) + 50;
    }

    ND bool canApply(const Items::Item *item) const override {
        return item->armorType == Items::ItemArmor || Enchantment::canApply(item);
    }
};

