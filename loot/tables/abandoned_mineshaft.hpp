#pragma once

#include "lce/items/itemsInit.hpp"
#include "loot/classes/Loot.hpp"
#include "loot/classes/LootFunctions.hpp"
#include "loot/classes/LootItem.hpp"
#include "loot/classes/LootTable.hpp"

namespace loot {

    using namespace lce::items;

    static constexpr auto abandoned_mineshaft =
            LootWrapper<
                    27, false,
                    TableWrapper<1, 1,
                                 LootItem<20, 1, 1, GOLDEN_APPLE.getState()>,
                                 LootItem< 1, 1, 1, ENCHANTED_GOLDEN_APPLE.getState()>,
                                 LootItem<30, 1, 1, NAME_TAG.getState()>,
                                 LootItem<10, 1, 1, ENCHANTED_BOOK.getState(), enchant_random_book>,
                                 LootItem< 5, 1, 1, IRON_PICKAXE.getState()>,
                                 LootItem< 5, 1, 1, AIR.getState()>
                                 >,
                    TableWrapper<2, 4,
                                 LootItem<10, 1, 5, IRON_INGOT.getState()>,
                                 LootItem< 5, 1, 3, GOLD_INGOT.getState()>,
                                 LootItem< 5, 4, 9, REDSTONE.getState()>,
                                 LootItem< 5, 4, 9, LAPIS_LAZULI.getState()>,
                                 LootItem< 3, 1, 2, DIAMOND.getState()>,
                                 LootItem<10, 3, 8, COAL.getState()>,
                                 LootItem<15, 1, 3, BREAD.getState()>,
                                 LootItem<10, 2, 4, MELON_SEEDS.getState()>,
                                 LootItem<10, 2, 4, PUMPKIN_SEEDS.getState()>,
                                 LootItem<10, 2, 4, BEETROOT_SEEDS.getState()>
                                 >,
                    TableWrapper<3, 3,
                                 LootItem<20, 4, 8, RAIL.getState()>,
                                 LootItem< 5, 1, 4, POWERED_RAIL.getState()>,
                                 LootItem< 5, 1, 4, DETECTOR_RAIL.getState()>,
                                 LootItem< 5, 1, 4, ACTIVATOR_RAIL.getState()>,
                                 LootItem<15, 1,16, TORCH.getState()>
                                 >
                    >::value;
}
