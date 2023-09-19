#pragma once

#include <cmath>

#include "enchantment.hpp"
#include "LegacyCubiomes/loot/base_classes/loot_classes.hpp"

#include "fastEnchantArray.hpp"

class EnchantmentHelperBook {
public:
    static constexpr int MAX_ENCHANT_LIST_SIZE = 29;
    static EnchantedBookEnchantsLookupTable BOOK_LEVEL_TABLE;

    static void setup() {
        BOOK_LEVEL_TABLE.setup();
    }

    class EnchantWithLevels {
    public:
        static void apply(uint64_t *rng, ItemStack *stack, int min, int max);
        static void apply(uint64_t *rng, ItemStack *stack, int level);
    };


private:
    static ELDataArray* buildEnchantmentList(uint64_t *rng, ItemStack *itemStackIn, int level);
    static void addRandomEnchantment(uint64_t *rng, ItemStack *itemStackIn, int level);
};







