#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentThorns : public Enchantment {
private:
    static constexpr inline int LEVELS_MIN[3] = {10, 30, 50};
    static constexpr inline int LEVELS_MAX[3] = {61, 71, 81};
public:

    EnchantmentThorns(std::string name, const Rarity *rarity) :
            Enchantment(std::move(name), rarity, &Type::ARMOR_CHEST, EnumName::THORNS, 3) {};

    int getMinCost(int enchantmentLevel) override {
        return LEVELS_MIN[enchantmentLevel];
    }

    int getMaxCost(int enchantmentLevel) override {
        return LEVELS_MAX[enchantmentLevel];
    }

    ND bool canApply(const Items::Item *item) const override {
        return item->getArmorType() == Items::ItemArmor || Enchantment::canApply(item);
    }
};

