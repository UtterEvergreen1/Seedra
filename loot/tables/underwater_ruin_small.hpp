#pragma once

#include "lce/items/itemsInit.hpp"
#include "loot/classes/Loot.hpp"

namespace loot {

    using namespace lce::items;

    MU static constexpr auto underwater_ruin_small
            = LootWrapper<
                    27,
                    TableWrapper<2, 8,
                                 LootItem<10, 1, 4, COAL.getState()>,
                                 LootItem< 2, 1, 1, STONE_AXE.getState()>,
                                 LootItem< 5, 1, 1, ROTTEN_FLESH.getState()>,
                                 LootItem< 1, 1, 1, EMERALD.getState()>,
                                 LootItem<10, 2, 3, WHEAT.getState()>
                                 >,
                    TableWrapper<1, 1,
                                 LootItem<1, 1, 1, LEATHER_TUNIC.getState()>,
                                 LootItem<1, 1, 1, GOLDEN_HELMET.getState()>,
                                 LootItem<5, 1, 1, FISHING_ROD.getState(), enchant_random_item>,
                                 LootItem<5, 1, 1, TREASURE_MAP.getState()>
                                 >
                    >::value;
}