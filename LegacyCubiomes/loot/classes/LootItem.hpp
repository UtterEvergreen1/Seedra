#pragma once

#include "LegacyCubiomes/mc/items.hpp"

class LootItem {
public:
    const Items::Item* item;
    int weight;
    LootItem() : item(nullptr), weight(0) {}
    LootItem(const Items::Item* item, int weight) : item(item), weight(weight) {}
};
