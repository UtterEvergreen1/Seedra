#pragma once

#include "lce/items/itemsInit.hpp"
#include "loot/classes/Loot.hpp"

namespace loot {

    using namespace lce::items;

    MU static constexpr auto stronghold_corridor_aquatic
            = LootWrapper<
                    27,
                    TableWrapper<2, 3,
                                 LootItem<50, 1, 1, ENDER_PEARL.getState()>,
                                 LootItem<15, 1, 3, EMERALD.getState()>,
                                 LootItem<15, 1, 3, DIAMOND.getState()>,
                                 LootItem<50, 1, 5, IRON_INGOT.getState()>,
                                 LootItem<25, 1, 3, GOLD_INGOT.getState()>,
                                 LootItem<25, 4, 9, REDSTONE.getState()>,
                                 LootItem<75, 1, 3, BREAD.getState()>,
                                 LootItem<75, 1, 3, APPLE.getState()>,
                                 LootItem<25, 1, 1, IRON_PICKAXE.getState()>,
                                 LootItem<25, 1, 1, IRON_SWORD.getState()>,
                                 LootItem<25, 1, 1, IRON_CHESTPLATE.getState()>,
                                 LootItem<25, 1, 1, IRON_HELMET.getState()>,
                                 LootItem<25, 1, 1, IRON_LEGGINGS.getState()>,
                                 LootItem<25, 1, 1, IRON_BOOTS.getState()>,
                                 LootItem< 5, 1, 1, GOLDEN_APPLE.getState()>,
                                 LootItem< 5, 1, 1, SADDLE.getState()>,
                                 LootItem< 5, 1, 1, IRON_HORSE_ARMOR.getState()>,
                                 LootItem< 5, 1, 1, GOLDEN_HORSE_ARMOR.getState()>,
                                 LootItem< 5, 1, 1, DIAMOND_HORSE_ARMOR.getState()>,
                                 LootItem< 6, 1, 1, ENCHANTED_BOOK.getState(), enchant_levels_book_30>
                                 >
                    >::value;
}