#pragma once

#include "lce/items/itemsInit.hpp"
#include "loot/classes/Loot.hpp"

namespace loot {

    using namespace lce::items;

    MU static constexpr auto underwater_ruin_big
            = LootWrapper<
                    27,
                    TableWrapper<2, 8,
                                 LootItem<10, 1, 4, COAL.getState()>,
                                 LootItem<10, 1, 3, GOLD_NUGGET.getState()>,
                                 LootItem< 1, 1, 1, EMERALD.getState()>,
                                 LootItem<10, 2, 3, WHEAT.getState()>
                                 >,
                    TableWrapper<1, 1,
                                 LootItem< 1, 1, 1, GOLDEN_APPLE.getState()>,
                                 LootItem< 5, 1, 1, ENCHANTED_BOOK.getState(), enchant_random_book>,
                                 LootItem< 1, 1, 1, LEATHER_TUNIC.getState()>,
                                 LootItem< 1, 1, 1, GOLDEN_HELMET.getState()>,
                                 LootItem< 5, 1, 1, FISHING_ROD.getState(), enchant_random_item>,
                                 LootItem<10, 1, 1, TREASURE_MAP.getState()>
                                 // items2.emplace_back(&POISONOUS_POTATO, 10); // zoom=1
                                 >
                    >::value;
}