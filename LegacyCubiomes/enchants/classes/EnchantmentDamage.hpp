#pragma once

#include "lce/items/item_ids.hpp"

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentDamage final : public Enchantment {
public:
    const int damageType;
    const std::vector<int8_t> BASE_ENCHANTABILITY = {1, 5, 5};
    const std::vector<int8_t> LEVEL_ENCHANTABILITY = {11, 8, 8};
    const std::vector<int8_t> THRESHOLD_ENCHANTABILITY = {20, 20, 20};


    EnchantmentDamage(std::string name, const Rarity* rarity, const int damage_type)
        : Enchantment(std::move(name), rarity, &Type::WEAPON, EnumName::DAMAGE, 5), damageType(damage_type){};

    int getMinCost(const int enchantmentLevel) override {
        return BASE_ENCHANTABILITY[this->damageType] + (enchantmentLevel - 1) * LEVEL_ENCHANTABILITY[this->damageType];
    }

    int getMaxCost(const int enchantmentLevel) override {
        return this->getMinCost(enchantmentLevel) + THRESHOLD_ENCHANTABILITY[this->damageType];
    }

    // checkCompatability
    ND bool canApplyTogether(const Enchantment* enchantment) const override {
        return enchantment->enumID != EnumName::DAMAGE;
    }

    ND bool canApply(const lce::items::Item* item) const override {
        return item->getID() == lce::items::ids::IRON_AXE_ID || Enchantment::canApply(item);
    }
};
