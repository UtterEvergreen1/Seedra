#pragma once

#include <cmath>

#include "enchantment.hpp"
#include "LegacyCubiomes/loot/base_classes/loot_classes.hpp"

#include "fastEnchantArray.hpp"

class EnchantmentHelperBook {
public:
    static EnchantedBookEnchantsLookupTable BOOK_LEVEL_TABLE;

    template<bool isAquatic>
    static void setup() {
        // Enchantment::registerEnchantments<isAquatic>();
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







