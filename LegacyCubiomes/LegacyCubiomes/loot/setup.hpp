#pragma once

#include "abandoned_mineshaft.hpp"
#include "blacksmith.hpp"
#include "buried_treasure.hpp"
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

namespace loot_tables {
    void setup() {
        loot_tables::AbandonedMineshaft::setup();
        loot_tables::Blacksmith::setup();
        loot_tables::BuriedTreasure::setup();
        loot_tables::DesertTemple::setup();
        loot_tables::EndCityTreasure::setup();
        loot_tables::Igloo::setup();
        loot_tables::JungleTemple::setup();
        loot_tables::JungleTempleDispenser::setup();
        loot_tables::NetherBridge::setup();
        loot_tables::SimpleDungeon::setup();
        loot_tables::SpawnBonusChest::setup();
        loot_tables::StrongholdLibrary::setup();
        loot_tables::Mansion::setup();
    }
}


