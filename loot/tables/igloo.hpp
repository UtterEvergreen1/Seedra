#pragma once

#include "lce/items/itemsInit.hpp"
#include "loot/classes/Loot.hpp"

namespace loot {

    using namespace lce::items;

    MU static constexpr auto igloo
            = LootWrapper<
                    27, false,
                    TableWrapper<2, 8,
                                 LootItem<15, 1, 3, APPLE.getState()>,
                                 LootItem<15, 1, 4, COAL.getState()>,
                                 LootItem<10, 1, 3, GOLD_NUGGET.getState()>,
                                 LootItem< 2, 1, 1, STONE_AXE.getState()>,
                                 LootItem<10, 1, 1, ROTTEN_FLESH.getState()>,
                                 LootItem< 1, 1, 1, EMERALD.getState()>,
                                 LootItem<10, 2, 3, WHEAT.getState()>>,
                    TableWrapper<1, 1,
                                 LootItem< 1, 1, 1, GOLDEN_APPLE.getState()>>
                    >::value;
}