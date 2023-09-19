#pragma once

#include <cmath>

#include "enchantment.hpp"
#include "weightedRandom.hpp"
#include "LegacyCubiomes/loot/base_classes/loot_classes.hpp"

class EnchantmentHelper {
public:
    static constexpr int MAX_ENCHANT_LIST_SIZE = 29;

    class EnchantWithLevels {
    public:
        template<bool isBook, bool allowTreasure>
        static void apply(uint64_t *rng, ItemStack *stack, int min, int max);

        template<bool isBook, bool allowTreasure>
        static void apply(uint64_t *rng, ItemStack *stack, int level);
    };

    class EnchantRandomly {
    public:
        std::vector<Enchantment> enchantments;

        template<bool isBook>
        void apply(uint64_t *rng, ItemStack *stack);
    };


private:
    template<bool isBook, bool allowTreasure>
    static std::vector<EnchantmentData> buildEnchantmentList(uint64_t *rng, ItemStack *itemStackIn, int level);

    static void removeIncompatible(std::vector<EnchantmentData>& enchDataList, EnchantmentData enchData);

    template<bool isBook, bool allowTreasure>
    static void addRandomEnchantment(uint64_t *rng, ItemStack *itemStackIn, int level);

    template<bool isBook, bool allowTreasure>
    static std::vector<EnchantmentData> getEnchantmentDataList(int enchantmentLevelIn, ItemStack *ItemStackIn);
};





