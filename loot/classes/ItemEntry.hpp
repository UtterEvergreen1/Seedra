#pragma once

#include "LootItem.hpp"
#include "UniformRoll.hpp"


class ItemEntry : public LootItem, public UniformRoll {
public:
    ItemEntry() = default;

    ItemEntry(const lce::Item* item, c_u8 weight) : LootItem(item, weight), UniformRoll(1, 1) {}

    ItemEntry(const lce::Item* item, LootFunction* lootFunction, c_u8 weight)
        : LootItem(item, {lootFunction}, weight), UniformRoll(1, 1) {}

    ItemEntry(const lce::Item* item, const std::vector<LootFunction*>& lootFunctions, c_u8 weight)
        : LootItem(item, lootFunctions, weight), UniformRoll(1, 1) {}

    ItemEntry(const lce::Item* item, c_u8 weight, c_u8 min, c_u8 max)
        : LootItem(item, weight), UniformRoll(min, max) {}

    ItemEntry(const lce::Item* item, LootFunction* lootFunction, c_u8 weight, c_u8 min,
              c_u8 max)
        : LootItem(item, {lootFunction}, weight), UniformRoll(min, max) {}

    ItemEntry(const lce::Item* item, const std::vector<LootFunction*>& lootFunctions, c_u8 weight,
              c_u8 min, c_u8 max)
        : LootItem(item, lootFunctions, weight), UniformRoll(min, max) {}
};
