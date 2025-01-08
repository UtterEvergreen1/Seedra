#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentThorns final : public Enchantment {
    static constexpr i8 LEVELS_MIN[3] = {10, 30, 50};
    static constexpr i8 LEVELS_MAX[3] = {61, 71, 81};

public:
    EnchantmentThorns(std::string name, const eRarity rarity)
        : Enchantment(std::move(name), rarity, &Type::ARMOR_CHEST, EnumName::THORNS, 3){};

    int getMinCost(c_int enchantmentLevel) override { return LEVELS_MIN[enchantmentLevel - 1]; }

    int getMaxCost(c_int enchantmentLevel) override { return LEVELS_MAX[enchantmentLevel - 1]; }

    ND bool canApply(const lce::items::Item* item) const override {
        return item->getItemType() == lce::items::ItemType::ItemArmor
               || Enchantment::canApply(item);
    }
};
