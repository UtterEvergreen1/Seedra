#pragma once

#include "lce/items/itemsInit.hpp"
#include "loot/classes/Loot.hpp"

namespace loot {

    using namespace lce::items;

    MU static constexpr auto end_city_treasure
            = LootWrapper<
                    27,
                    TableWrapper<2, 6,
                                 LootItem< 5, 2, 7, DIAMOND.getState()>,
                                 LootItem<10, 4, 8, IRON_INGOT.getState()>,
                                 LootItem<15, 2, 7, GOLD_INGOT.getState()>,
                                 LootItem< 2, 2, 6, EMERALD.getState()>,
                                 LootItem< 5, 1,10, BEETROOT_SEEDS.getState()>,
                                 LootItem< 3, 1, 1, SADDLE.getState()>,
                                 LootItem< 1, 1, 1, IRON_HORSE_ARMOR.getState()>,
                                 LootItem< 1, 1, 1, GOLDEN_HORSE_ARMOR.getState()>,
                                 LootItem< 1, 1, 1, DIAMOND_HORSE_ARMOR.getState()>,

                                 LootItem< 3, 1, 1, DIAMOND_SWORD.getState(), enchant_levels_item_20_39>,
                                 LootItem< 3, 1, 1, DIAMOND_BOOTS.getState(), enchant_levels_item_20_39>,
                                 LootItem< 3, 1, 1, DIAMOND_CHESTPLATE.getState(), enchant_levels_item_20_39>,
                                 LootItem< 3, 1, 1, DIAMOND_LEGGINGS.getState(), enchant_levels_item_20_39>,
                                 LootItem< 3, 1, 1, DIAMOND_HELMET.getState(), enchant_levels_item_20_39>,
                                 LootItem< 3, 1, 1, DIAMOND_PICKAXE.getState(), enchant_levels_item_20_39>,
                                 LootItem< 3, 1, 1, DIAMOND_SHOVEL.getState(), enchant_levels_item_20_39>,
                                 LootItem< 3, 1, 1, IRON_SWORD.getState(), enchant_levels_item_20_39>,
                                 LootItem< 3, 1, 1, IRON_BOOTS.getState(), enchant_levels_item_20_39>,
                                 LootItem< 3, 1, 1, IRON_CHESTPLATE.getState(), enchant_levels_item_20_39>,
                                 LootItem< 3, 1, 1, IRON_LEGGINGS.getState(), enchant_levels_item_20_39>,
                                 LootItem< 3, 1, 1, IRON_HELMET.getState(), enchant_levels_item_20_39>,
                                 LootItem< 3, 1, 1, IRON_PICKAXE.getState(), enchant_levels_item_20_39>,
                                 LootItem< 3, 1, 1, IRON_SHOVEL.getState(), enchant_levels_item_20_39>
                                 >>::value;
}