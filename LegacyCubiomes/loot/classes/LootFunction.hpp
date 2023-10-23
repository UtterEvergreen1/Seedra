#pragma once
#include "ItemStack.hpp"

class ItemStack;
///TODO: Make a function for treasure maps to check for a buried treasure on the map and if not found,
/// replace it with NAUTILUS_CORE
class LootFunction {
protected:
    static constexpr int MAX_ENCHANT_LIST_SIZE = 29;

public:
    virtual void apply(ItemStack& itemStack, RNG& random) = 0;
};