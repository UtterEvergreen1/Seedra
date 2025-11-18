#pragma once

/**
 * @enum StructureVariant
 * @brief Represents the different variants of structures used in world generation.
 *
 * This enumeration defines various structure variants, such as villages and mineshafts,
 * with each variant corresponding to a specific biome or type.
 */
enum class StructureVariant {
    SV_NONE = 0, ///< No structure variant.

    // Village structure variants
    SV_Village_Plains = 0, ///< Village variant for plains biome.
    SV_Village_Desert = 1, ///< Village variant for desert biome.
    SV_Village_Savanna = 2, ///< Village variant for savanna biome.
    SV_Village_Taiga = 3, ///< Village variant for taiga biome.

    // Mineshaft structure variants
    SV_Mineshaft_Normal = 0, ///< Standard mineshaft variant.
    SV_Mineshaft_Mesa = 1 ///< Mineshaft variant for mesa biome.
};
