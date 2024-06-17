#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentArrowFire final : public Enchantment {
public:
    EnchantmentArrowFire(std::string name, const Rarity* rarity)
        : Enchantment(std::move(name), rarity, &Type::BOW, EnumName::ARROW_FIRE, 1){};

    int getMinCost(c_int enchantmentLevel) override { return 20; }

    int getMaxCost(c_int enchantmentLevel) override { return 50; }
};
