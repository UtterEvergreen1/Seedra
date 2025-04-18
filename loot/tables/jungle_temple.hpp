#pragma once

#include "lce/items/itemsInit.hpp"
#include "loot/classes/Loot.hpp"

namespace loot {

    using namespace lce::items;

    MU static constexpr auto jungle_temple
            = LootWrapper<
                    27,
                    TableWrapper<2, 6,
                                 LootItem< 15, 1, 3, EMERALD.getState()>,
                                 LootItem< 50, 1, 5, IRON_INGOT.getState()>,
                                 LootItem< 75, 2, 7, GOLD_INGOT.getState()>,
                                 LootItem< 10, 1, 3, EMERALD.getState()>,
                                 LootItem<100, 4, 6, BONE.getState()>,
                                 LootItem< 80, 3, 7, ROTTEN_FLESH.getState()>,
                                 LootItem< 15, 1, 1, SADDLE.getState()>,
                                 LootItem<  5, 1, 1, IRON_HORSE_ARMOR.getState()>,
                                 LootItem<  5, 1, 1, GOLDEN_HORSE_ARMOR.getState()>,
                                 LootItem<  5, 1, 1, DIAMOND_HORSE_ARMOR.getState()>,
                                 LootItem<  6, 1, 1, ENCHANTED_BOOK.getState(), enchant_levels_book_30>
                                 >
                    >::value;
}