#pragma once

#include "lce/items/itemsInit.hpp"
#include "loot/classes/Loot.hpp"

namespace loot {

    using namespace lce::items;

    MU static constexpr auto spawn_bonus_chest
            = LootWrapper<
                    27,
                    TableWrapper<1, 1,
                                 LootItem<1, 1, 5, JUNGLE_SAPLING.getState()>
                                 >,
                    TableWrapper<1, 1,
                                 LootItem<1, 1, 5, MELON_SEEDS.getState()>
                                 >,
                    TableWrapper<1, 1,
                                 LootItem<1, 1, 5, PUMPKIN_SEEDS.getState()>
                                 >,
                    TableWrapper<1, 1,
                                 LootItem<1, 1, 5, BEETROOT_SEEDS.getState()>
                                 >,
                    TableWrapper<1, 1,
                                 LootItem<1, 1, 1, STONE_AXE.getState()>,
                                 LootItem<3, 1, 1, WOODEN_AXE.getState()>
                                 >,
                    TableWrapper<1, 1,
                                 LootItem<1, 1, 1, STONE_PICKAXE.getState()>,
                                 LootItem<3, 1, 1, WOODEN_PICKAXE.getState()>
                                 >,
                    TableWrapper<3, 3,
                                 LootItem<5, 1, 2, APPLE.getState()>,
                                 LootItem<3, 1, 2, BREAD.getState()>,
                                 LootItem<3, 1, 2, RAW_SALMON.getState()>
                                 >,
                    TableWrapper<4, 4,
                                 LootItem<10, 1, 8, STICK.getState()>,
                                 LootItem<10, 1,12, OAK_WOOD_PLANK.getState()>,
                                 LootItem<10, 1, 3, OAK_WOOD.getState(), metadata_0_3>,
                                 LootItem<10, 1, 3, ACACIA_WOOD.getState(), metadata_0_1>
                                 >
                    >::value;
}