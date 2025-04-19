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
    /**
     * @brief Sets the world size for all structures in the Placement namespace.
     *
     * This function adjusts the world size for various structures, ensuring that
     * their placement logic is consistent with the specified world size.
     *
     * @param worldSize The size of the world, which determines placement behavior.
     */
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