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
#include "shipwreck_map.hpp"
#include "shipwreck_supply.hpp"
#include "shipwreck_treasure.hpp"
#include "underwater_ruin_big.hpp"
#include "underwater_ruin_small.hpp"

namespace loot {
    template<bool isAquatic>
    void setup() {
        loot::AbandonedMineshaft::setup();
        loot::Blacksmith::setup();
        loot::DesertTemple::setup();
        loot::EndCityTreasure::setup();
        loot::Igloo::setup();
        loot::JungleTemple::setup();
        loot::JungleTempleDispenser::setup();
        loot::NetherBridge::setup();
        loot::SimpleDungeon::setup();
        loot::Mansion::setup();
        loot::SpawnBonusChest::setup();
        loot::StrongholdLibrary::setup();
        loot::StrongholdCorridor<isAquatic>::setup();

        if constexpr (isAquatic) {
            loot::BuriedTreasure::setup();
            loot::UnderwaterRuinBig::setup();
            loot::UnderwaterRuinSmall::setup();
            loot::ShipwreckTreasure::setup();
            loot::ShipwreckSupply::setup();
            loot::ShipwreckMap::setup();
        }
    }
} // namespace loot
