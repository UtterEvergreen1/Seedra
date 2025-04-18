#pragma once

#include "lce/items/itemsInit.hpp"
#include "loot/classes/Loot.hpp"

namespace loot {

    using namespace lce::items;

    MU static constexpr auto shipwreck_supply
            = LootWrapper<
                    27,
                    TableWrapper<3, 10,
                                 LootItem<8, 1,12, PAPER.getState()>,
                                 LootItem<7, 2, 6, POTATO.getState()>,
                                 LootItem<7, 2, 6, POISONOUS_POTATO.getState()>,
                                 LootItem<7, 4, 8, CARROT.getState()>,
                                 LootItem<7, 8,21, WHEAT.getState()>,
                                 LootItem<6, 2, 8, COAL.getState()>,
                                 LootItem<5, 5,24, ROTTEN_FLESH.getState()>,
                                 LootItem<2, 1, 3, PUMPKIN.getState()>,
                                 LootItem<3, 1, 5, GUNPOWDER.getState()>,
                                 LootItem<1, 1, 2, TNT.getState()>,
                                 LootItem<3, 1, 1, LEATHER_HELMET.getState(), enchant_random_item>,
                                 LootItem<3, 1, 1, LEATHER_TUNIC.getState(), enchant_random_item>,
                                 LootItem<3, 1, 1, LEATHER_PANTS.getState(), enchant_random_item>,
                                 LootItem<3, 1, 1, LEATHER_BOOTS.getState(), enchant_random_item>
                                 >
                    >::value;
}