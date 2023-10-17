#pragma once

#include <utility>

#include "LegacyCubiomes/loot/classes/LootItem.hpp"
#include "LegacyCubiomes/loot/classes/UniformRoll.hpp"

class ItemEntry : public LootItem, public UniformRoll {
public:
    ItemEntry() = default;

    ItemEntry(const Items::Item *item, uint8_t weight) :
            LootItem(item, weight), UniformRoll(1, 1) {}

    ItemEntry(const Items::Item *item, LootFunction* lootFunction, uint8_t weight) :
            LootItem(item, {lootFunction}, weight), UniformRoll(1, 1) {}

    ItemEntry(const Items::Item *item, const std::vector<LootFunction *>& lootFunctions, uint8_t weight) :
            LootItem(item, lootFunctions, weight), UniformRoll(1, 1) {}

    ItemEntry(const Items::Item *item, uint8_t weight, uint8_t min, uint8_t max) :
            LootItem(item, weight), UniformRoll(min, max) {}

    ItemEntry(const Items::Item *item, LootFunction* lootFunction, uint8_t weight, uint8_t min,
              uint8_t max) :
            LootItem(item, {lootFunction}, weight), UniformRoll(min, max) {}

    ItemEntry(const Items::Item *item, const std::vector<LootFunction *>& lootFunctions, uint8_t weight, uint8_t min,
              uint8_t max) :
            LootItem(item, lootFunctions, weight), UniformRoll(min, max) {}
};
