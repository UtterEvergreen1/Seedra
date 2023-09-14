#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentDamage : public Enchantment {
public:

    const int damageType;
    const std::vector<int> BASE_ENCHANTABILITY  = {1, 5, 5};
    const std::vector<int> LEVEL_ENCHANTABILITY  = {11, 8, 8};
    const std::vector<int> THRESHOLD_ENCHANTABILITY  = {20, 20, 20};


    EnchantmentDamage(std::string name, const Rarity *rarity, int damage_type) :
        Enchantment(std::move(name), rarity, &Type::WEAPON, EnumName::DAMAGE, 5), damageType(damage_type) {};

    int getMinEnchantability(int enchantmentLevel) override {
        return BASE_ENCHANTABILITY[this->damageType] + (enchantmentLevel - 1) * LEVEL_ENCHANTABILITY[this->damageType];
    }

    int getMaxEnchantability(int enchantmentLevel) override {
        return this->getMinEnchantability(enchantmentLevel) + THRESHOLD_ENCHANTABILITY[this->damageType];
    }

    ND bool canApplyTogether(const Enchantment *enchantment) const override {
        return enchantment->enumID != EnumName::DAMAGE;
    }

    ND bool canApply(const Items::Item *item) const override {
        return item->id == Items::IRON_AXE_ID || Enchantment::canApply(item);
    }
};

