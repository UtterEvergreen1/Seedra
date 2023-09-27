#pragma once

#include <cmath>
#include <cassert>

#include "enchantment.hpp"
#include "weightedRandom.hpp"
#include "fastBookEnchantArray.hpp"

#include "LegacyCubiomes/loot/classes/Loot.hpp"


class EnchantmentHelper {
public:
    static constexpr int MAX_ENCHANT_LIST_SIZE = 29;
    static EnchantedBookEnchantsLookupTable BOOK_LEVEL_TABLE;

    static void setup(CONSOLE console, LCEVERSION version) {
        Enchantment::registerEnchantments();
        try {
            setConsoleAndVersion(console, version);
            BOOK_LEVEL_TABLE.setup();
        }
        catch(std::exception& exception) {
            #ifdef DEV_MODE
            std::cout << exception.what();
            #endif
            exit(-1);
        }
    }

    /**
     * Sets the order of the enchantments according to the console and version
     * @param console the LCE console type
     * @param version the LCE version
     */
    static void setConsoleAndVersion(CONSOLE console, LCEVERSION version);


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





