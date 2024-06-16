#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentTridentImpaler final : public Enchantment {
    static constexpr inline int8_t LEVELS_MIN[5] = {17, 24, 31, 38, 45};

public:
    EnchantmentTridentImpaler(std::string name, const Rarity *rarity) :
            Enchantment(std::move(name), rarity, &Type::TRIDENT,
                        EnumName::TRIDENT_IMPALER, 5) {};

    int getMinCost(const int enchantmentLevel) override { return LEVELS_MIN[enchantmentLevel - 1]; }

    int getMaxCost(const int enchantmentLevel) override { return 50; }
};

