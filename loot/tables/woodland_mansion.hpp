#pragma once

#include "lce/items/itemsInit.hpp"
#include "loot/classes/Loot.hpp"

namespace loot {

    using namespace lce::items;

    MU static constexpr auto woodland_mansion
            = LootWrapper<
                    27,
                    TableWrapper<1, 3,
                                 LootItem<100, 1, 1, LEAD.getState()>,
                                 LootItem< 75, 1, 1, GOLDEN_APPLE.getState()>,
                                 LootItem< 10, 1, 1, ENCHANTED_GOLDEN_APPLE.getState()>,
                                 LootItem< 15, 1, 1, DISC_13.getState()>,
                                 LootItem< 15, 1, 1, DISC_CAT.getState()>,
                                 LootItem<100, 1, 1, NAME_TAG.getState()>,
                                 LootItem< 50, 1, 1, CHAINMAIL_CHESTPLATE.getState()>,
                                 LootItem< 75, 1, 1, DIAMOND_HOE.getState()>,
                                 LootItem< 25, 1, 1, DIAMOND_CHESTPLATE.getState()>,
                                 LootItem< 60, 1, 1, ENCHANTED_BOOK.getState(), enchant_random_book>
                                 >,
                    TableWrapper<1, 4,
                                 LootItem< 50, 1, 4, IRON_INGOT.getState()>,
                                 LootItem< 25, 1, 4, GOLD_INGOT.getState()>,
                                 LootItem<100, 1, 1, BREAD.getState()>,
                                 LootItem<100, 1, 4, WHEAT.getState()>,
                                 LootItem< 50, 1, 1, BUCKET.getState()>,
                                 LootItem< 75, 1, 4, REDSTONE.getState()>,
                                 LootItem< 75, 1, 4, COAL.getState()>,
                                 LootItem< 50, 2, 4, MELON_SEEDS.getState()>,
                                 LootItem< 50, 2, 4, PUMPKIN_SEEDS.getState()>,
                                 LootItem< 50, 2, 4, BEETROOT_SEEDS.getState()>
                                 >,
                    TableWrapper<3, 3,
                                 LootItem<50, 1, 8, BONE.getState()>,
                                 LootItem<50, 1, 8, GUNPOWDER.getState()>,
                                 LootItem<50, 1, 8, ROTTEN_FLESH.getState()>,
                                 LootItem<50, 1, 8, STRING.getState()>
                                 >
                    >::value;
}