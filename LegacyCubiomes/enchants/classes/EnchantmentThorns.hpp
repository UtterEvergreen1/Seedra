#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentThorns final : public Enchantment {
    static constexpr inline int8_t LEVELS_MIN[3] = {10, 30, 50};
    static constexpr inline int8_t LEVELS_MAX[3] = {61, 71, 81};

public:
    EnchantmentThorns(std::string name, const Rarity* rarity)
        : Enchantment(std::move(name), rarity, &Type::ARMOR_CHEST, EnumName::THORNS, 3){};

    int getMinCost(const int enchantmentLevel) override { return LEVELS_MIN[enchantmentLevel - 1]; }

    int getMaxCost(const int enchantmentLevel) override { return LEVELS_MAX[enchantmentLevel - 1]; }

    ND bool canApply(const Items::Item* item) const override {
        return item->getItemType() == Items::ItemArmor || Enchantment::canApply(item);
    }
};
