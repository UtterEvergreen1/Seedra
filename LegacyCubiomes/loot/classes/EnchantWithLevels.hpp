#pragma once
#include "LegacyCubiomes/enchants/enchantmentHelper.hpp"
#include "LegacyCubiomes/enchants/fastBookEnchantArray.hpp"
#include "LegacyCubiomes/enchants/weightedRandom.hpp"
#include "LegacyCubiomes/utils/MathHelper.hpp"
#include "LootFunction.hpp"
#include "UniformRoll.hpp"


class EnchantWithLevels : public LootFunction {
protected:
    UniformRoll randomLevel;

    explicit EnchantWithLevels(UniformRoll roll);
};


class EnchantWithLevelsBook : public EnchantWithLevels {
public:
    explicit EnchantWithLevelsBook(UniformRoll levelRange);

    explicit EnchantWithLevelsBook(int level);

    void apply(ItemStack& itemStack, RNG& random) final;

private:
    static ELDataArray* buildEnchantmentList(const ItemStack& itemStackIn, RNG& rng, int level);
};


class EnchantWithLevelsItem : public EnchantWithLevels {
public:
    explicit EnchantWithLevelsItem(UniformRoll levelRange);

    explicit EnchantWithLevelsItem(int level);

    void apply(ItemStack& itemStack, RNG& random) final;

private:
    static inline void addRandomEnchantment(RNG& rng, ItemStack& itemStackIn, int level) {
        EnchDataVec_t enchantmentList = buildEnchantmentList(itemStackIn, rng, level);

        for (const auto& enchantmentData: enchantmentList) {
            itemStackIn.addEnchantment(enchantmentData.obj, enchantmentData.level);
        }
    }

    static EnchDataVec_t buildEnchantmentList(const ItemStack& itemStackIn, RNG& rng, int level);

    static EnchDataVec_t getEnchantmentDataList(int enchantmentLevelIn, const ItemStack& ItemStackIn);

    static void removeIncompatible(EnchDataVec_t& enchDataList, EnchantmentData enchData);
};
