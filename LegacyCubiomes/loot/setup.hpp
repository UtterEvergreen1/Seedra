#pragma once

#include "abandoned_mineshaft.hpp"
#include "blacksmith.hpp"
#include "desert_temple.hpp"
#include "end_city_treasure.hpp"
#include "igloo_chest.hpp"
#include "jungle_temple.hpp"
#include "jungle_temple_dispenser.hpp"
#include "nether_bridge.hpp"
#include "simple_dungeon.hpp"
#include "spawn_bonus_chest.hpp"
#include "stronghold_corridor.hpp"
#include "stronghold_crossing.hpp"
#include "stronghold_library.hpp"
#include "woodland_mansion.hpp"

#include "buried_treasure.hpp"
#include "underwater_ruin_big.hpp"
#include "underwater_ruin_small.hpp"
#include "shipwreck_treasure.hpp"
#include "shipwreck_supply.hpp"
#include "shipwreck_map.hpp"

namespace loot_tables {
    template<bool isAquatic>
    void setup() {
        loot_tables::AbandonedMineshaft::setup();
        loot_tables::Blacksmith::setup();
        loot_tables::DesertTemple::setup();
        loot_tables::EndCityTreasure::setup();
        loot_tables::Igloo::setup();
        loot_tables::JungleTemple::setup();
        loot_tables::JungleTempleDispenser::setup();
        loot_tables::NetherBridge::setup();
        loot_tables::SimpleDungeon::setup();
        loot_tables::Mansion::setup();
        loot_tables::SpawnBonusChest::setup();
        loot_tables::StrongholdLibrary::setup();
        loot_tables::StrongholdCorridor<isAquatic>::setup();

        if constexpr (isAquatic) {
            loot_tables::BuriedTreasure::setup();
            loot_tables::UnderwaterRuinBig::setup();
            loot_tables::UnderwaterRuinSmall::setup();
            loot_tables::ShipwreckTreasure::setup();
            loot_tables::ShipwreckSupply::setup();
            loot_tables::ShipwreckMap::setup();
        }
    }
}

