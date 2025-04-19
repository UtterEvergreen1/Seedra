#pragma once

#include "lce/items/itemsInit.hpp"
#include "loot/classes/Loot.hpp"

namespace loot {

    using namespace lce::items;

    MU static constexpr auto shipwreck_treasure
            = LootWrapper<
                    27,
                    TableWrapper<3, 6,
                                 LootItem<90, 1, 5, IRON_INGOT.getState()>,
                                 LootItem<10, 1, 5, GOLD_INGOT.getState()>,
                                 LootItem<40, 1, 5, EMERALD.getState()>,
                                 LootItem< 5, 1, 1, DIAMOND.getState()>,
                                 LootItem< 5, 1, 1, BOTTLE_O_ENCHANTING.getState()>
                                 >,
                    TableWrapper<2, 5,
                                 LootItem<50, 1, 10, IRON_NUGGET.getState()>,
                                 LootItem<10, 1, 10, GOLD_NUGGET.getState()>,
                                 LootItem<20, 1, 10, LAPIS_LAZULI.getState()>
                                 >
                    >::value;
}