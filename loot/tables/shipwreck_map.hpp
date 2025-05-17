#pragma once

#include "lce/items/itemsInit.hpp"
#include "loot/classes/Loot.hpp"

namespace loot {

    using namespace lce::items;

    MU static constexpr auto shipwreck_map
            = LootWrapper<
                    27, false,
                    TableWrapper<1, 1,
                                 LootItem<1, 1, 1, TREASURE_MAP.getState()>
                                 >,

                    TableWrapper<3, 3,
                                 LootItem< 1, 1, 1, COMPASS.getState()>,
                                 LootItem< 1, 1, 1, MAP.getState()>,
                                 LootItem< 1, 1, 1, CLOCK.getState()>,
                                 LootItem<20, 1,10, PAPER.getState()>,
                                 LootItem<10, 1, 5, FEATHER.getState()>,
                                 LootItem< 5, 1, 5, BOOK.getState()>
                                 >
                    >::value;
}