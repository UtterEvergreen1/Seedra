#pragma once

#include "lce/items/itemsInit.hpp"
#include "loot/classes/Loot.hpp"

namespace loot {

    using namespace lce::items;

    MU static constexpr auto blacksmith
            = LootWrapper<
                    27, false,
                    TableWrapper<3, 8,
                                 LootItem< 3, 1, 3, DIAMOND.getState()>,
                                 LootItem<10, 1, 5, IRON_INGOT.getState()>,
                                 LootItem< 5, 1, 3, GOLD_INGOT.getState()>,
                                 LootItem<15, 1, 3, BREAD.getState()>,
                                 LootItem<15, 1, 3, APPLE.getState()>,
                                 LootItem< 5, 1, 1, IRON_PICKAXE.getState()>,
                                 LootItem< 5, 1, 1, IRON_SWORD.getState()>,
                                 LootItem< 5, 1, 1, IRON_CHESTPLATE.getState()>,
                                 LootItem< 5, 1, 1, IRON_HELMET.getState()>,
                                 LootItem< 5, 1, 1, IRON_LEGGINGS.getState()>,
                                 LootItem< 5, 1, 1, IRON_BOOTS.getState()>,
                                 LootItem< 5, 3, 7, OBSIDIAN.getState()>,
                                 LootItem< 5, 3, 7, OAK_SAPLING.getState()>,
                                 LootItem< 3, 1, 1, SADDLE.getState()>,
                                 LootItem< 1, 1, 1, IRON_HORSE_ARMOR.getState()>,
                                 LootItem< 1, 1, 1, GOLDEN_HORSE_ARMOR.getState()>,
                                 LootItem< 1, 1, 1, DIAMOND_HORSE_ARMOR.getState()>>
                    >::value;
}