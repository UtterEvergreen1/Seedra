#pragma once

// generation
#include "LegacyCubiomes/structures/gen/mineshaft/mineshaft.hpp"
#include "LegacyCubiomes/structures/gen/netherfortress/netherfortress.hpp"
#include "LegacyCubiomes/structures/gen/stronghold/stronghold.hpp"
#include "LegacyCubiomes/structures/gen/village/village.hpp"


// placement
#include "LegacyCubiomes/structures/placement/DynamicStructures.hpp"
#include "LegacyCubiomes/structures/placement/StaticStructures.hpp"
#include "LegacyCubiomes/structures/placement/mineshaft.hpp"
#include "LegacyCubiomes/structures/placement/netherfortress.hpp"
#include "LegacyCubiomes/structures/placement/stronghold.hpp"

// rolls
#include "LegacyCubiomes/structures/build/village.hpp"
#include "LegacyCubiomes/structures/rolls/mineshaft.hpp"
#include "LegacyCubiomes/structures/rolls/stronghold.hpp"


// generation


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
