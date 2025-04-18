#pragma once

#include "lce/items/itemsInit.hpp"
#include "loot/classes/Loot.hpp"

namespace loot {

    using namespace lce::items;

    MU static constexpr auto village_two_room_house
            = LootWrapper<
                    27,
                    TableWrapper<6, 8,
                                 LootItem<10, 5, 8, POTATO.getState()>,
                                 LootItem<10, 4, 8, CARROT.getState()>,
                                 LootItem<15, 8,12, WHEAT.getState()>,
                                 LootItem< 5, 2, 4, WHEAT_SEEDS.getState()>,
                                 LootItem< 5, 5, 8, BEETROOT.getState()>,
                                 LootItem< 1, 1, 1, WOODEN_HOE.getState()>
                                 >
                    >::value;
}