#pragma once

// generation
#include "LegacyCubiomes/structures/generation/mineshaft/mineshaft.hpp"
#include "LegacyCubiomes/structures/generation/netherfortress/netherfortress.hpp"
#include "LegacyCubiomes/structures/generation/stronghold/stronghold.hpp"
#include "LegacyCubiomes/structures/generation/village/StructureVillagePieces.hpp"


// placement
#include "LegacyCubiomes/structures/placement/DynamicStructures.hpp"
#include "LegacyCubiomes/structures/placement/mineshaft.hpp"
#include "LegacyCubiomes/structures/placement/netherfortress.hpp"
#include "LegacyCubiomes/structures/placement/StaticStructures.hpp"
#include "LegacyCubiomes/structures/placement/stronghold.hpp"

// rolls
#include "LegacyCubiomes/structures/rolls/mineshaft.hpp"
#include "LegacyCubiomes/structures/rolls/stronghold.hpp"
#include "LegacyCubiomes/structures/rolls/village.hpp"



// generation


// placement

namespace Placement {
    static void setWorldSize(WORLDSIZE worldSize) {
        Mansion::setWorldSize(worldSize);
        Monument::setWorldSize(worldSize);
        BuriedTreasure::setWorldSize(worldSize);
        Shipwreck::setWorldSize(worldSize);
        Outpost::setWorldSize(worldSize);
    }
}

// rolls
