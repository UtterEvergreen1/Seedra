#pragma once

#include "lce/items/itemsInit.hpp"
#include "loot/classes/Loot.hpp"

namespace loot {

    using namespace lce::items;

    MU static constexpr auto buried_treasure
            = LootWrapper<
                    27, true,
                    TableWrapper<1, 1,
                                 LootItem<1, 1, 1, HEART_OF_THE_SEA.getState()>>,
                    TableWrapper<5, 12,
                                 LootItem< 5, 1, 5, PRISMARINE_CRYSTALS.getState()>,
                                 LootItem<20, 3, 5, IRON_INGOT.getState()>,
                                 LootItem<10, 1, 5, GOLD_INGOT.getState()>,
                                 LootItem<10, 1, 2, TNT.getState()>,
                                 LootItem<15, 1, 1, DIAMOND.getState()>,
                                 LootItem< 5, 1, 1, DISC_WAIT.getState()>,
                                 LootItem< 5, 1, 1, DISC_MELLOHI.getState()>,
                                 LootItem<10, 1, 1, NAME_TAG.getState()>,
                                 LootItem<20, 1, 1, CHAINMAIL_CHESTPLATE.getState()>,
                                 LootItem<20, 1, 1, CHAINMAIL_HELMET.getState()>,
                                 LootItem<20, 1, 1, CHAINMAIL_LEGGINGS.getState()>,
                                 LootItem<20, 1, 1, CHAINMAIL_BOOTS.getState()>,
                                 LootItem< 5, 1, 2, BOOK_AND_QUILL.getState()>,
                                 LootItem<10, 1, 3, LEAD.getState()>,
                                 LootItem< 3, 1, 1, BOTTLE_O_ENCHANTING.getState()>,
                                 LootItem<15, 1, 1, POTION_OF_WATER_BREATHING.getState()>,
                                 LootItem<10, 1, 1, POTION_OF_REGENERATION.getState()>,
                                 LootItem< 1, 1, 1, CAKE.getState()>>
                    >::value;
}