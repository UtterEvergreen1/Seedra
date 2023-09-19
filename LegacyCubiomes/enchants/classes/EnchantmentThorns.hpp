#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentThorns : public Enchantment {
public:

    EnchantmentThorns(std::string name, const Rarity *rarity) :
            Enchantment(std::move(name), rarity, &Type::ARMOR_CHEST, EnumName::THORNS, 3) {};

    int getMinCost(int enchantmentLevel) override {
        return 20 * enchantmentLevel - 10;
    }

    int getMaxCost(int enchantmentLevel) override {
        return Enchantment::getMinCost(enchantmentLevel) + 50;
    }

    ND bool canApply(const Items::Item *item) const override {
        return item->getArmorType() == Items::ItemArmor || Enchantment::canApply(item);
    }
};

