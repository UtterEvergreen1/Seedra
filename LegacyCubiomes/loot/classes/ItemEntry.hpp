#pragma once

#include "LegacyCubiomes/loot/classes/LootItem.hpp"
#include "LegacyCubiomes/loot/classes/UniformRoll.hpp"

class ItemEntry : public LootItem, public UniformRoll {
public:
    ItemEntry() = default;
    ItemEntry(const Items::Item* item, uint8_t weight) :
        LootItem(item, weight), UniformRoll(1, 1) {}
    ItemEntry(const Items::Item* item, uint8_t weight, uint8_t min, uint8_t max) :
        LootItem(item, weight), UniformRoll(min, max) {}
};
