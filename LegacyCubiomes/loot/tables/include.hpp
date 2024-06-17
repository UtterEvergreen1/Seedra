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
        AbandonedMineshaft::setup();
        Blacksmith::setup();
        DesertTemple::setup();
        EndCityTreasure::setup();
        Igloo::setup();
        JungleTemple::setup();
        JungleTempleDispenser::setup();
        NetherBridge::setup();
        SimpleDungeon::setup();
        Mansion::setup();
        SpawnBonusChest::setup();
        StrongholdCrossing::setup();
        StrongholdLibrary::setup();
        StrongholdCorridor<isAquatic>::setup();

        if constexpr (isAquatic) {
            BuriedTreasure::setup();
            UnderwaterRuinBig::setup();
            UnderwaterRuinSmall::setup();
            ShipwreckTreasure::setup();
            ShipwreckSupply::setup();
            ShipwreckMap::setup();
        }
    }
}
