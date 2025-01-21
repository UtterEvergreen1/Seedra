#pragma once

// generation
#include "gen/netherfortress/netherfortress.hpp"
#include "gen/mineshaft/mineshaft.hpp"
#include "gen/stronghold/stronghold.hpp"
#include "gen/village/village.hpp"

// placement
#include "placement/DynamicStructures.hpp"
#include "placement/StaticStructures.hpp"
#include "placement/mineshaft.hpp"
#include "placement/stronghold.hpp"

// placement
namespace Placement {
    static void setWorldSize(const lce::WORLDSIZE worldSize) {
        Stronghold::setWorldSize(worldSize);
        Feature::setWorldSize(worldSize);
        Village<false>::setWorldSize(worldSize);
        Village<true>::setWorldSize(worldSize);
        OceanRuin::setWorldSize(worldSize);
        Mansion::setWorldSize(worldSize);
        Monument::setWorldSize(worldSize);
        BuriedTreasure::setWorldSize(worldSize);
        Shipwreck::setWorldSize(worldSize);
        Outpost::setWorldSize(worldSize);
    }
} // namespace Placement

// rolls
