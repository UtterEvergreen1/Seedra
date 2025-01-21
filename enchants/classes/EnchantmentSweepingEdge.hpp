#pragma once

#include "enchants/enchantment.hpp"


#ifdef INCLUDE_JAVA
class EnchantmentSweepingEdge : public Enchantment {
public:
    EnchantmentSweepingEdge(std::string name, const eRarity rarity)
        : Enchantment(std::move(name), rarity, &Type::WEAPON, EnumName::SWEEPING, 3){};

    int getMinEnchantability(c_int enchantmentLevel) override { return 5 + (enchantmentLevel - 1) * 9; }

    int getMaxEnchantability(c_int enchantmentLevel) override { return getMinEnchantability(enchantmentLevel) + 15; }
};
#endif
