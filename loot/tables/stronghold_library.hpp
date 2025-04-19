#pragma once

#include "lce/items/itemsInit.hpp"
#include "loot/classes/Loot.hpp"

namespace loot {

    using namespace lce::items;

    MU static constexpr auto stronghold_library
            = LootWrapper<
                    27,
                    TableWrapper<2, 10,
                                 LootItem<100, 1, 3, BOOK.getState()>,
                                 LootItem<100, 2, 7, PAPER.getState()>,
                                 LootItem<  5, 1, 1, MAP.getState()>,
                                 LootItem<  5, 1, 1, COMPASS.getState()>,
                                 LootItem< 60, 1, 1, ENCHANTED_BOOK.getState(), enchant_levels_book_30>
                                 >
                    >::value;
}