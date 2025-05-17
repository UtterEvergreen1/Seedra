#pragma once

#include "lce/items/itemsInit.hpp"
#include "loot/classes/Loot.hpp"

namespace loot {

    using namespace lce::items;

    MU static constexpr auto jungle_temple_dispenser
            = LootWrapper<
                    9, false,
                    TableWrapper<1, 2,
                                 LootItem<30, 2, 7, ARROW.getState()>
                                 >
                    >::value;
}