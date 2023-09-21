#pragma once

#include <cmath>

#include "enchantment.hpp"
#include "weightedRandom.hpp"
#include "fastBookEnchantArray.hpp"

#include "LegacyCubiomes/loot/base_classes/loot_classes.hpp"

constexpr int MAX_ENCHANT_LIST_SIZE = 33;


struct EnchantWithLevelsItem {
    void removeIncompatible(EnchDataVec_t& enchDataList, EnchantmentData enchData);

    template<bool allowTreasure>
    void apply(uint64_t *rng, ItemStack *stack, int min, int max);
    template<bool allowTreasure>
    void apply(uint64_t *rng, ItemStack *stack, int level);
    template<bool allowTreasure>
    EnchDataVec_t buildEnchantmentList(uint64_t *rng, ItemStack *itemStackIn, int level);
    template<bool allowTreasure>
    void addRandomEnchantment(uint64_t *rng, ItemStack *itemStackIn, int level);
    template<bool allowTreasure>
    EnchDataVec_t getEnchantmentDataList(int enchantmentLevelIn, ItemStack *ItemStackIn);
};


struct EnchantWithLevelsBook {
    EnchantedBookEnchantsLookupTable BOOK_LEVEL_TABLE = EnchantedBookEnchantsLookupTable();
    ELDataArray* buildEnchantmentList(uint64_t *rng, ItemStack *itemStackIn, int level);

    template<bool allowTreasure>
    void apply(uint64_t *rng, ItemStack *stack, int level);
};


struct EnchantRandomlyItem {
    void apply(uint64_t *rng, ItemStack *stack);
};


struct EnchantRandomlyBook {
    void apply(uint64_t *rng, ItemStack *stack);
};



template<bool isAquatic>
class EnchantmentHelper {
public:
    EnchantWithLevelsItem enchantWithLevelsItem = EnchantWithLevelsItem();
    EnchantWithLevelsBook enchantWithLevelsBook = EnchantWithLevelsBook();
    EnchantRandomlyItem enchantRandomlyItem = EnchantRandomlyItem();
    EnchantRandomlyBook enchantRandomlyBook = EnchantRandomlyBook();

    EnchantmentHelper() {
        Enchantment::registerEnchantments<isAquatic>();
        enchantWithLevelsBook.BOOK_LEVEL_TABLE.setup();
    }

    void deallocate() {
        enchantWithLevelsBook.BOOK_LEVEL_TABLE.deallocate();
        Enchantment::REGISTRY.clear();
        Enchantment::count = 0;
        Enchantment::isSetup = false;
        Enchantment::Type::ALL_ITERABLE.clear();
    }




};





