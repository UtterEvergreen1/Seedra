#pragma once

#include "lce/items/item_ids.hpp"

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentDamage final : public Enchantment {
public:
    c_int damageType;
    const std::vector<i8> BASE_ENCHANTABILITY = {1, 5, 5};
    const std::vector<i8> LEVEL_ENCHANTABILITY = {11, 8, 8};
    const std::vector<i8> THRESHOLD_ENCHANTABILITY = {20, 20, 20};


    EnchantmentDamage(std::string name, const eRarity rarity, c_int damage_type)
        : Enchantment(std::move(name), rarity, &Type::WEAPON, EnumName::DAMAGE, 5), damageType(damage_type){};

    int getMinCost(c_int enchantmentLevel) override {
        return BASE_ENCHANTABILITY[this->damageType] + (enchantmentLevel - 1) * LEVEL_ENCHANTABILITY[this->damageType];
    }

    int getMaxCost(c_int enchantmentLevel) override {
        return this->getMinCost(enchantmentLevel) + THRESHOLD_ENCHANTABILITY[this->damageType];
    }

    /// checkCompatability
    ND bool canApplyTogether(const Enchantment* enchantment) const override {
        return enchantment->enumID != EnumName::DAMAGE;
    }

    ND bool canApply(const lce::items::Item* item) const override {
        return item->getID() == lce::items::ids::IRON_AXE_ID || Enchantment::canApply(item);
    }
};
