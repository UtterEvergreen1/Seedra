#pragma once

#include "LegacyCubiomes/mc/items.hpp"
#include "LootFunction.hpp"
#include <utility>

class LootFunction;
class LootItem {
public:
    const Items::Item* item;
    ///stores the functions it calls for the item
    const std::vector<LootFunction*> functions;
    int weight;

    LootItem() : item(nullptr), weight(0) {}

    LootItem(const Items::Item* item, int weight) : item(item), weight(weight) {}

    LootItem(const Items::Item* item, const std::vector<LootFunction*>& lootFunctions, int weight)
        : item(item), functions(lootFunctions), weight(weight) {}
};