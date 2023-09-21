#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentTridentImpaler : public Enchantment {
public:
    EnchantmentTridentImpaler(std::string name, const Rarity *rarity) :
            Enchantment(std::move(name), rarity, &Type::TRIDENT,
                        EnumName::TRIDENT_IMPALER, 5) {};

    int getMinCost(int enchantmentLevel) override {
        return enchantmentLevel * 7 + 10;
    }

    int getMaxCost(int enchantmentLevel) override {
        return 50;
    }
};

