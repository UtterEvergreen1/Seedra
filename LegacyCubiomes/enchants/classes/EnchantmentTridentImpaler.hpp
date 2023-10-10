#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentTridentImpaler : public Enchantment {
private:
    static constexpr inline int8_t LEVELS_MIN[5] = {17, 24, 31, 38, 45};
public:
    EnchantmentTridentImpaler(std::string name, const Rarity *rarity) :
            Enchantment(std::move(name), rarity, &Type::TRIDENT,
                        EnumName::TRIDENT_IMPALER, 5) {};

    int getMinCost(int enchantmentLevel) override {
        return LEVELS_MIN[enchantmentLevel];
    }

    int getMaxCost(int enchantmentLevel) override {
        return 50;
    }
};

