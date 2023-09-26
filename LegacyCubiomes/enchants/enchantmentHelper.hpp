#pragma once

#include <cmath>

#include "enchantment.hpp"
#include "weightedRandom.hpp"
#include "fastBookEnchantArray.hpp"

#include "LegacyCubiomes/loot/base_classes/loot_classes.hpp"


class EnchantmentHelper {
public:
    static constexpr int MAX_ENCHANT_LIST_SIZE = 29;
    static EnchantedBookEnchantsLookupTable BOOK_LEVEL_TABLE;

    static void setup(CONSOLE console, LCEVERSION version) {
        Enchantment::registerEnchantments();
        Enchantment::setConsoleAndVersion(console, version);
        BOOK_LEVEL_TABLE.setup();
    }

    static void deallocate() {
        BOOK_LEVEL_TABLE.deallocate();
        Enchantment::REGISTRY.clear();
        Enchantment::count = 0;
        Enchantment::isSetup = false;
        Enchantment::Type::ALL_ITERABLE.clear();
    }


    struct EnchantWithLevelsItem {
        static void apply(uint64_t *rng, ItemStack *stack, int min, int max);
        static void apply(uint64_t *rng, ItemStack *stack, int level);
        static EnchDataVec_t buildEnchantmentList(uint64_t *rng, ItemStack *itemStackIn, int level);
        static void removeIncompatible(EnchDataVec_t& enchDataList, EnchantmentData enchData);
        static void addRandomEnchantment(uint64_t *rng, ItemStack *itemStackIn, int level);
        static EnchDataVec_t getEnchantmentDataList(int enchantmentLevelIn, ItemStack *ItemStackIn);
    };


    struct EnchantWithLevelsBook {
        static void apply(uint64_t *rng, ItemStack *stack, int level);
        static ELDataArray* buildEnchantmentList(uint64_t *rng, ItemStack *itemStackIn, int level);
    };


    struct EnchantRandomlyItem {
        static void apply(uint64_t *rng, ItemStack *stack);
    };


    struct EnchantRandomlyBook {
        static void apply(uint64_t *rng, ItemStack *stack);
    };

};





