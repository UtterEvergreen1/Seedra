#pragma once

#include "lce/items/items.hpp"
#include "LootFunction.hpp"


class LootFunction;
class LootItem {
public:
    const lce::items::Item* item;

    ///stores the functions it calls for the item
    const std::vector<LootFunction*> functions;
    int weight;

    LootItem() : item(nullptr), weight(0) {}

    LootItem(const lce::items::Item* item, const int weight) : item(item), weight(weight) {}

    LootItem(const lce::items::Item* item, const std::vector<LootFunction*>& lootFunctions, const int weight)
        : item(item), functions(lootFunctions), weight(weight) {}
};