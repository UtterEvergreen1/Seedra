#pragma once

#include "LegacyCubiomes/loot/classes/LootItem.hpp"
#include "LegacyCubiomes/loot/classes/UniformRoll.hpp"

class ItemEntry : public LootItem, public UniformRoll {
public:
    ItemEntry() = default;
    ItemEntry(const Items::Item* item, int weight) : LootItem(item, weight), UniformRoll(1, 1) {}
    ItemEntry(const Items::Item* item, int weight, int min, int max) : LootItem(item, weight), UniformRoll(min, max) {}
};
