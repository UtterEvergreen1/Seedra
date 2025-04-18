#pragma once

#include "lce/items/itemsInit.hpp"
#include "loot/classes/Loot.hpp"

namespace loot {

    using namespace lce::items;

    MU static constexpr auto nether_bridge
            = LootWrapper<
                    27,
                    TableWrapper<2, 4,
                                 LootItem< 5, 1, 3, DIAMOND.getState()>,
                                 LootItem< 5, 1, 5, IRON_INGOT.getState()>,
                                 LootItem<15, 1, 3, GOLD_INGOT.getState()>,
                                 LootItem< 5, 1, 1, GOLDEN_SWORD.getState()>,
                                 LootItem< 5, 1, 1, GOLDEN_CHESTPLATE.getState()>,
                                 LootItem< 5, 1, 1, FLINT_AND_STEEL.getState()>,
                                 LootItem< 5, 3, 7, NETHER_WART.getState()>,
                                 LootItem<10, 1, 1, SADDLE.getState()>,
                                 LootItem< 8, 1, 1, GOLDEN_HORSE_ARMOR.getState()>,
                                 LootItem< 5, 1, 1, IRON_HORSE_ARMOR.getState()>,
                                 LootItem< 3, 1, 1, DIAMOND_HORSE_ARMOR.getState()>,
                                 LootItem< 2, 2, 4, OBSIDIAN.getState()>
                                 >
                    >::value;
}