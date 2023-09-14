#pragma once


#pragma once

#include "base_classes/loot.hpp"

using namespace Items;

namespace loot_tables {
    class VillageTwoRoomHouse : public Loot<VillageTwoRoomHouse> {
    public:
        static void setup();
    };

    void VillageTwoRoomHouse::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&POTATO,              3, 1, 3);
        items.emplace_back(&IRON_INGOT,          10, 1, 5);
        items.emplace_back(&GOLD_INGOT,          15, 2, 7);
        items.emplace_back(&EMERALD,              2, 1, 3);
        items.emplace_back(&BONE,                20, 4, 6);
        items.emplace_back(&ROTTEN_FLESH,        16, 3, 7);
        items.emplace_back(&SADDLE,                     3);
        items.emplace_back(&IRON_HORSE_ARMOR,           1);
        items.emplace_back(&GOLDEN_HORSE_ARMOR,         1);
        items.emplace_back(&DIAMOND_HORSE_ARMOR,        1);
        items.emplace_back(&ENCHANTED_BOOK,             1);
        lootTables.emplace_back(items,            6, 8, 0);

        maxItemsPossible = 6;
    }
}








/*
{
  "pools": [
    {
      "rolls": {
        "min": 6,
        "max": 8
      },
      "entries": [
        {
          "type": "item",
          "name": "minecraft:potato",
          "functions": [
            {
              "function": "set_count",
              "count": {
                "min": 5,
                "max": 8
              }
            }
          ],
          "weight": 10
        },
        {
          "type": "item",
          "name": "minecraft:carrot",
          "functions": [
            {
              "function": "set_count",
              "count": {
                "min": 4,
                "max": 8
              }
            }
          ],
          "weight": 10
        },
        {
          "type": "item",
          "name": "minecraft:wheat",
          "functions": [
            {
              "function": "set_count",
              "count": {
                "min": 8,
                "max": 12
              }
            }
          ],
          "weight": 15
        },
        {
          "type": "item",
          "name": "minecraft:wheat_seeds",
          "functions": [
            {
              "function": "set_count",
              "count": {
                "min": 2,
                "max": 4
              }
            }
          ],
          "weight": 5
        },
        {
          "type": "item",
          "name": "minecraft:beetroot",
          "functions": [
            {
              "function": "set_count",
              "count": {
                "min": 5,
                "max": 8
              }
            }
          ],
          "weight": 5
        },
        {
          "type": "item",
          "name": "minecraft:wooden_hoe",
          "functions": [
            {
              "function": "set_count",
              "count": 1
            }
          ],
          "weight": 1
        }
      ]
    }
  ]
}
 */