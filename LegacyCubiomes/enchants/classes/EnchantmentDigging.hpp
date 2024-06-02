#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentDigging final : public Enchantment {
    static constexpr inline int8_t LEVELS_MIN[5] = {1, 11, 21, 31, 41};
    static constexpr inline int8_t LEVELS_MAX[5] = {61, 71, 81, 91, 101};

public:
    EnchantmentDigging(std::string name, const Rarity* rarity)
        : Enchantment(std::move(name), rarity, &Type::DIGGER, EnumName::DIGGING, 5){};

    int getMinCost(const int enchantmentLevel) override { return LEVELS_MIN[enchantmentLevel - 1]; }

    int getMaxCost(const int enchantmentLevel) override { return LEVELS_MAX[enchantmentLevel - 1]; }

    ND bool canApply(const lce::items::Item* item) const override {
        return item->getID() == lce::items::ids::SHEARS_ID || Enchantment::canApply(item);
    }
};
