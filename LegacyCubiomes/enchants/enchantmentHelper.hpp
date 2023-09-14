#pragma once

#include <cmath>

#include "enchantment.hpp"
#include "LegacyCubiomes/utils/weightedRandom.hpp"
#include "LegacyCubiomes/loot/base_classes/loot_classes.hpp"

class EnchantmentHelper {
public:
    class EnchantWithLevels {
    public:
        static void apply(uint64_t *rng, ItemStack *stack, int min, int max, bool isBook, bool isTreasure);
        static void apply(uint64_t *rng, ItemStack *stack, int level, bool isBook, bool isTreasure);
    };
    class EnchantRandomly {
    public:
        std::vector<Enchantment> enchantments;
        void apply(uint64_t *rng, ItemStack *stack);
    };


private:
    static std::vector<EnchantmentData> buildEnchantmentList(uint64_t *rng, ItemStack *itemStackIn, int level, bool isBook, bool allowTreasure);
    static void removeIncompatible(std::vector<EnchantmentData>& enchDataList, EnchantmentData enchData);
    static void addRandomEnchantment(uint64_t *rng, ItemStack *itemStackIn, int level, bool isBook, bool allowTreasure);
    static std::vector<EnchantmentData> getEnchantmentDataList(int enchantmentLevelIn, ItemStack *ItemStackIn, bool isBook, bool allowTreasure);
};







