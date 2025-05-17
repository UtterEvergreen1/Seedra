#pragma once

#include "lce/items/itemsInit.hpp"
#include "loot/classes/Loot.hpp"

namespace loot {

    using namespace lce::items;

    MU static constexpr auto stronghold_crossing
            = LootWrapper<
                    27, false,
                    TableWrapper<1, 4,
                                 LootItem<50, 1, 5, IRON_INGOT.getState()>,
                                 LootItem<25, 1, 3, GOLD_INGOT.getState()>,
                                 LootItem<25, 4, 9, REDSTONE.getState()>,
                                 LootItem<50, 3, 8, COAL.getState()>,
                                 LootItem<75, 1, 3, BREAD.getState()>,
                                 LootItem<75, 1, 3, APPLE.getState()>,
                                 LootItem< 5, 1, 1, IRON_PICKAXE.getState()>,
                                 LootItem< 6, 1, 1, ENCHANTED_BOOK.getState(), enchant_levels_book_30>,
                                 LootItem<75, 1, 3, INK_SACK.getState()>
                                 >
                    >::value;
}