#pragma once

#include "lce/processor.hpp"
#include "lce/enums.hpp"

/**
 * @file enums.hpp
 * @brief Defines enumerations and utility functions for Minecraft versions and structures.
 */

/// Minecraft console versions
enum class LCEVERSION : i8 {
    AQUATIC = 0, ///< Represents the Aquatic update.
    ELYTRA = 1, ///< Represents the Elytra update.
    BOUNTIFUL = 2, ///< Represents the Bountiful update.
    HORSE = 3, ///< Represents the Horse update.
    POTIONS = 4, ///< Represents the Potions update.
    ADVENTURE = 5, ///< Represents the Adventure update.
    PILLAGE = 6, ///< Represents the Pillage update.
    NONE = 7 ///< Represents no specific version.
};

/// Minecraft world generators
enum class WORLDGENERATOR : i8 {
    DEFAULT = 0, ///< Default world generator.
    FLAT = 1 ///< Flat world generator.
};

/// Minecraft Java versions
enum MCVERSION : i8 {
    MC_1_0 = 0, ///< Version 1.0
    MC_1_1 = 1, ///< Version 1.1
    MC_1_2 = 2, ///< Version 1.2
    MC_1_3 = 3, ///< Version 1.3
    MC_1_4 = 4, ///< Version 1.4
    MC_1_5 = 5, ///< Version 1.5
    MC_1_6 = 6, ///< Version 1.6
    MC_1_7 = 7, ///< Version 1.7
    MC_1_8 = 8, ///< Version 1.8
    MC_1_9 = 9, ///< Version 1.9
    MC_1_10 = 10, ///< Version 1.10
    MC_1_11 = 11, ///< Version 1.11
    MC_1_12 = 12, ///< Version 1.12
    MC_1_13 = 13, ///< Version 1.13
    MC_1_14 = 14 ///< Version 1.14
};

/// Types of structures in Minecraft
enum class StructureType : u8 {
    DesertPyramid, ///< Desert Pyramid structure.
    JungleTemple, ///< Jungle Temple structure.
    JunglePyramid = JungleTemple, ///< Alias for Jungle Temple.
    SwampHut, ///< Swamp Hut structure.
    Igloo, ///< Igloo structure.
    FEATURE_NUM, ///< Number of features (internal use).
    Village, ///< Village structure.
    OceanRuin, ///< Ocean Ruin structure.
    Mansion, ///< Woodland Mansion structure.
    Monument, ///< Ocean Monument structure.
    Treasure, ///< Buried Treasure structure.
    Shipwreck, ///< Shipwreck structure.
    Outpost, ///< Pillager Outpost structure.
    Mineshaft, ///< Mineshaft structure.
    Fortress, ///< Nether Fortress structure.
    EndCity, ///< End City structure.
    EndGateway, ///< End Gateway structure.
    NONE ///< No specific structure.
};

/**
 * @brief Converts an LCEVERSION enum value to its string representation.
 * @param version The LCEVERSION value to convert.
 * @return A string representing the LCEVERSION.
 */
MU static std::string LceVersionToString(const LCEVERSION version) {
    switch (version) {
        case LCEVERSION::PILLAGE:
            return "PS4_VP";
        case LCEVERSION::AQUATIC:
            return "AQUATIC";
        case LCEVERSION::ELYTRA:
            return "ELYTRA";
        case LCEVERSION::BOUNTIFUL:
            return "BOUNTIFUL";
        case LCEVERSION::HORSE:
            return "HORSE";
        case LCEVERSION::POTIONS:
            return "POTIONS";
        case LCEVERSION::ADVENTURE:
            return "ADVENTURE";
        case LCEVERSION::NONE:
            return "NONE";
    }
}

inline LCEVERSION strToLCEVERSION(const std::string& version) {
    if (version == "PS4_VP") {
        return LCEVERSION::PILLAGE;
    }
    if (version == "AQUATIC") {
        return LCEVERSION::AQUATIC;
    }
    if (version == "ELYTRA") {
        return LCEVERSION::ELYTRA;
    }
    if (version == "BOUNTIFUL") {
        return LCEVERSION::BOUNTIFUL;
    }
    if (version == "HORSE") {
        return LCEVERSION::HORSE;
    }
    if (version == "POTIONS") {
        return LCEVERSION::POTIONS;
    }
    if (version == "ADVENTURE") {
        return LCEVERSION::ADVENTURE;
    }
    return LCEVERSION::NONE;
}

/**
 * @brief Converts a StructureType enum value to its string representation.
 * @param type The StructureType value to convert.
 * @return A string representing the StructureType.
 */
MU static std::string getStructureName(const StructureType type) {
    switch (type) {
        case StructureType::DesertPyramid:
            return "DesertPyramid";
        case StructureType::JunglePyramid:
            return "JunglePyramid";
        case StructureType::SwampHut:
            return "SwampHut";
        case StructureType::Igloo:
            return "Igloo";
        case StructureType::Village:
            return "Village";
        case StructureType::OceanRuin:
            return "OceanRuin";
        case StructureType::Mansion:
            return "Mansion";
        case StructureType::Monument:
            return "Monument";
        case StructureType::Treasure:
            return "Buried Treasure";
        case StructureType::Shipwreck:
            return "Shipwreck";
        case StructureType::Outpost:
            return "Outpost";
        case StructureType::Mineshaft:
            return "Mineshaft";
        case StructureType::Fortress:
            return "Fortress";
        case StructureType::EndCity:
            return "EndCity";
        case StructureType::EndGateway:
            return "EndGateway";
        case StructureType::NONE:
            return "NONE";
        case StructureType::FEATURE_NUM:
            return "FEATURE_NUM";
        default:
            return "N/A";
    }
}

/**
 * @brief Maps an LCEVERSION value to its corresponding MCVERSION.
 * @param lceVersionIn The LCEVERSION value to map.
 * @return The corresponding MCVERSION value.
 */
MU static MCVERSION getMCVersion(const LCEVERSION lceVersionIn) {
    switch (lceVersionIn) {
        case LCEVERSION::AQUATIC:
        default:
            return MCVERSION::MC_1_13;
        case LCEVERSION::ELYTRA:
            return MCVERSION::MC_1_12;
        case LCEVERSION::BOUNTIFUL:
            return MCVERSION::MC_1_8;
        case LCEVERSION::HORSE:
            return MCVERSION::MC_1_6;
        case LCEVERSION::POTIONS:
            return MCVERSION::MC_1_2;
        case LCEVERSION::ADVENTURE:
            return MCVERSION::MC_1_0;
        case LCEVERSION::PILLAGE:
            return MCVERSION::MC_1_14;
        case LCEVERSION::NONE:
            return MCVERSION::MC_1_0;
    }
}


/**
 * @brief Maps an LCEVERSION value to its corresponding MCVERSION.
 * @tparam LceVersion The LCEVERSION value to map.
 * @return The corresponding MCVERSION value.
 */
template<LCEVERSION LceVersion>
MU static MCVERSION getMCVersion() {
    switch (LceVersion) {
        case LCEVERSION::AQUATIC:
        default:
            return MCVERSION::MC_1_13;
        case LCEVERSION::ELYTRA:
            return MCVERSION::MC_1_12;
        case LCEVERSION::BOUNTIFUL:
            return MCVERSION::MC_1_8;
        case LCEVERSION::HORSE:
            return MCVERSION::MC_1_6;
        case LCEVERSION::POTIONS:
            return MCVERSION::MC_1_2;
        case LCEVERSION::ADVENTURE:
            return MCVERSION::MC_1_0;
        case LCEVERSION::PILLAGE:
            return MCVERSION::MC_1_14;
        case LCEVERSION::NONE:
            return MCVERSION::MC_1_0;
    }
}


/**
 * @brief Converts an MCVERSION enum value to its string representation.
 * @param version The MCVERSION value to convert.
 * @return A string representing the MCVERSION.
 */
MU static std::string mcVersionToString(const MCVERSION version) {
    switch (version) {
        case MCVERSION::MC_1_0:
            return "1.0";
        case MCVERSION::MC_1_1:
            return "1.1";
        case MCVERSION::MC_1_2:
            return "1.2";
        case MCVERSION::MC_1_3:
            return "1.3";
        case MCVERSION::MC_1_4:
            return "1.4";
        case MCVERSION::MC_1_5:
            return "1.5";
        case MCVERSION::MC_1_6:
            return "1.6";
        case MCVERSION::MC_1_7:
            return "1.7";
        case MCVERSION::MC_1_8:
            return "1.8";
        case MCVERSION::MC_1_9:
            return "1.9";
        case MCVERSION::MC_1_10:
            return "1.10";
        case MCVERSION::MC_1_11:
            return "1.11";
        case MCVERSION::MC_1_12:
            return "1.12";
        case MCVERSION::MC_1_13:
            return "1.13";
        case MCVERSION::MC_1_14:
            return "1.14";
        default:
            return "NONE";
    }
}
