#include "enchantmentRegistry.hpp"


void EnchantmentRegistry::clear() {
    for (const Enchantment* value: allValues) {
        delete value;
    }
    allValues.clear();
    sortedRegistry.clear();
}