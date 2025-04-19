#pragma once

#include "lce/processor.hpp"

/**
 * @enum GenerationStep
 * @brief Represents the various steps in the generation process for structures like villages and strongholds.
 *
 * This enumeration defines the different stages of generation for specific structures,
 * such as villages and strongholds, with each step corresponding to a particular level
 * of detail or completion in the generation process.
 */
enum GenerationStep : i8 {
    GS_NONE = 0, ///< No generation step.

    // Village generation steps
    GS_Village_NONE = 0, ///< No village generation step.
    GS_Village_Valid = 1, ///< Generates the layout up until the village becomes valid.
    GS_Village_Blacksmith = 2, ///< Generates the layout up to the blacksmith.
    GS_Village_Layout = 3, ///< Generates the full layout of the village.
    GS_Village_Full = 4, ///< Generates the full layout and calculates if the village has more than two components.

    // Stronghold generation steps
    GS_Stronghold_NONE = 0, ///< No stronghold generation step.
    GS_Stronghold_Portal = 1, ///< Generates the layout up to the portal room (Y level will be inaccurate).
    GS_Stronghold_Layout = 2, ///< Generates the full layout of the stronghold (Y level will be inaccurate).
    GS_Stronghold_Full = 3, ///< Generates the full layout and calculates the Y level for all pieces.
};