#pragma once

#include "enchantment.hpp"
#include "fastBookEnchantArray.hpp"


class EnchantmentHelper {
public:
    static EnchantedBookEnchantsLookupTable BOOK_LEVEL_TABLE;

    /**
     * Sets up the enchantments in the order according to the console and version
     * @param console the LCE console type
     * @param version the LCE version
     */
    static void setup(const lce::CONSOLE console, const LCEVERSION version) {
        Enchantment::registerEnchantments();
        setConsoleAndVersion(console, version);
    }

    /**
     * Sets the order of the enchantments according to the console and version
     * @param console the LCE console type
     * @param version the LCE version
     */
    static void setConsoleAndVersion(lce::CONSOLE console, LCEVERSION version);

    /**
     * Deallocates the book level table and registry
     */
    static void deallocate();
};
