#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentDurability final : public Enchantment {
    static constexpr inline int8_t LEVELS_MIN[3] = {5, 13, 21};
    static constexpr inline int8_t LEVELS_MAX[3] = {61, 71, 81};

public:
    EnchantmentDurability(std::string name, const Rarity* rarity)
        : Enchantment(std::move(name), rarity, &Type::BREAKABLE, EnumName::DURABILITY, 3){};

    int getMinCost(const int enchantmentLevel) override { return LEVELS_MIN[enchantmentLevel - 1]; }

    int getMaxCost(const int enchantmentLevel) override { return LEVELS_MAX[enchantmentLevel - 1]; }

    ND bool canApply(const lce::items::Item* item) const override {
        return item->isDamageable() || Enchantment::canApply(item);
    }
};
