#pragma once

#include "lce/items/itemsInit.hpp"
#include "loot/classes/Loot.hpp"

namespace loot {

    using namespace lce::items;

    MU static constexpr auto desert_temple
            = LootWrapper<
                    27, false,
                    TableWrapper<2, 4,
                                 LootItem< 5, 1, 3, DIAMOND.getState()>,
                                 LootItem<15, 1, 5, IRON_INGOT.getState()>,
                                 LootItem<15, 2, 7, GOLD_INGOT.getState()>,
                                 LootItem<15, 1, 3, EMERALD.getState()>,
                                 LootItem<25, 4, 6, BONE.getState()>,
                                 LootItem<25, 1, 3, SPIDER_EYE.getState()>,
                                 LootItem<25, 3, 7, ROTTEN_FLESH.getState()>,
                                 LootItem<20, 1, 1, SADDLE.getState()>,
                                 LootItem<15, 1, 1, IRON_HORSE_ARMOR.getState()>,
                                 LootItem<10, 1, 1, GOLDEN_HORSE_ARMOR.getState()>,
                                 LootItem< 5, 1, 1, DIAMOND_HORSE_ARMOR.getState()>,
                                 LootItem<20, 1, 1, ENCHANTED_BOOK.getState(), enchant_random_book>,
                                 LootItem<20, 1, 1, GOLDEN_APPLE.getState()>,
                                 LootItem< 2, 1, 1, ENCHANTED_GOLDEN_APPLE.getState()>,
                                 LootItem<15, 1, 1, AIR.getState()>>,
                    TableWrapper<4, 4,
                                 LootItem<10, 1, 8, BONE.getState()>,
                                 LootItem<10, 1, 8, GUNPOWDER.getState()>,
                                 LootItem<10, 1, 8, ROTTEN_FLESH.getState()>,
                                 LootItem<10, 1, 8, STRING.getState()>,
                                 LootItem<10, 1, 8, SAND.getState()>>
                    >::value;
}