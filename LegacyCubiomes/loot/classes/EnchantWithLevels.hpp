#pragma once

#include "LegacyCubiomes/enchants/fastBookEnchantArray.hpp"
#include "LegacyCubiomes/enchants/weightedRandom.hpp"
#include "LootFunction.hpp"
#include "UniformRoll.hpp"


class EnchantWithLevels : public LootFunction {
protected:
    UniformRoll randomLevel;

    explicit EnchantWithLevels(UniformRoll roll);
};


class EnchantWithLevelsBook final : public EnchantWithLevels {
public:
    explicit EnchantWithLevelsBook(UniformRoll levelRange);
    explicit EnchantWithLevelsBook(int level);
    void apply(ItemStack& itemStack, RNG& random) override;

private:
    static ELDataArray* buildEnchantmentList(const ItemStack& itemStackIn, RNG& rng, int level);
};


class EnchantWithLevelsItem final : public EnchantWithLevels {
public:
    explicit EnchantWithLevelsItem(UniformRoll levelRange);
    explicit EnchantWithLevelsItem(int level);
    void apply(ItemStack& itemStack, RNG& random) override;

private:
    static void addRandomEnchantment(RNG& rng, ItemStack& itemStackIn, c_int level) {
        for (c_auto& enchantmentData: buildEnchantmentList(itemStackIn, rng, level)) {
            itemStackIn.addEnchantment(enchantmentData.obj, enchantmentData.level);
        }
    }

    static EnchDataVec_t buildEnchantmentList(const ItemStack& itemStackIn, RNG& rng, int level);
    static EnchDataVec_t getEnchantmentDataList(int enchantmentLevelIn, const ItemStack& ItemStackIn);
    static void removeIncompatible(EnchDataVec_t& enchDataList, EnchantmentData enchData);
};
