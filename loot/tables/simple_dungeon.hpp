#pragma once

#include "lce/items/itemsInit.hpp"
#include "loot/classes/Loot.hpp"

namespace loot {

    using namespace lce::items;

    MU static constexpr auto simple_dungeon
            = LootWrapper<
                    27,
                    TableWrapper<1, 3,
                                 LootItem<20, 1, 1, SADDLE.getState()>,
                                 LootItem<15, 1, 1, GOLDEN_APPLE.getState()>,
                                 LootItem< 2, 1, 1, ENCHANTED_GOLDEN_APPLE.getState()>,
                                 LootItem<15, 1, 1, DISC_13.getState()>,
                                 LootItem<15, 1, 1, DISC_CAT.getState()>,
                                 LootItem<20, 1, 1, NAME_TAG.getState()>,
                                 LootItem<10, 1, 1, GOLDEN_HORSE_ARMOR.getState()>,
                                 LootItem<15, 1, 1, IRON_HORSE_ARMOR.getState()>,
                                 LootItem< 5, 1, 1, DIAMOND_HORSE_ARMOR.getState()>,
                                 LootItem<10, 1, 1, ENCHANTED_BOOK.getState(), enchant_random_book>
                                 >,
                    TableWrapper<1, 4,
                                 LootItem<10, 1, 4, IRON_INGOT.getState()>,
                                 LootItem< 5, 1, 4, GOLD_INGOT.getState()>,
                                 LootItem<20, 1, 1, BREAD.getState()>,
                                 LootItem<20, 1, 4, WHEAT.getState()>,
                                 LootItem<10, 1, 1, BUCKET.getState()>,
                                 LootItem<15, 1, 4, REDSTONE.getState()>,
                                 LootItem<15, 1, 4, COAL.getState()>,
                                 LootItem<10, 2, 4, MELON_SEEDS.getState()>,
                                 LootItem<10, 2, 4, PUMPKIN_SEEDS.getState()>,
                                 LootItem<10, 2, 4, BEETROOT_SEEDS.getState()>
                                 >,
                    TableWrapper<3, 3,
                                 LootItem<10, 1, 8, BONE.getState()>,
                                 LootItem<10, 1, 8, GUNPOWDER.getState()>,
                                 LootItem<10, 1, 8, ROTTEN_FLESH.getState()>,
                                 LootItem<10, 1, 8, STRING.getState()>
                                 >
                    >::value;
}