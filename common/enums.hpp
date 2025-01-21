#pragma once

#include "lce/processor.hpp"
#include "lce/enums.hpp"


/// Minecraft console versions
// TODO: this code sucks make it better
enum class LCEVERSION : i8 {
    AQUATIC = 0,
    ELYTRA = 1,
    BOUNTIFUL = 2,
    HORSE = 3,
    POTIONS = 4,
    ADVENTURE = 5,
    // PISTON,
    // GENERATION, // not supported yet

    PILLAGE = 6,
    NONE = 7
};

/// Minecraft Java versions
enum MCVERSION : i8 {
    MC_1_0 = 0, // <=1.0 Experimental!
    MC_1_1 = 1,
    MC_1_2 = 2,
    MC_1_3 = 3,
    MC_1_4 = 4,
    MC_1_5 = 5,
    MC_1_6 = 6,
    MC_1_7 = 7,
    MC_1_8 = 8,
    MC_1_9 = 9,
    MC_1_10 = 10,
    MC_1_11 = 11,
    MC_1_12 = 12,
    MC_1_13 = 13,
    MC_1_14 = 14
};


enum class StructureType : u8 {
    DesertPyramid,
    JungleTemple,
    JunglePyramid = JungleTemple,
    SwampHut,
    Igloo,
    FEATURE_NUM,
    Village,
    OceanRuin,
    Mansion,
    Monument,
    Treasure,
    Shipwreck,
    Outpost,
    Mineshaft,
    Fortress,
    EndCity,
    EndGateway,
    NONE
};


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
        default:
            return "NONE";
    }
}


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
        default:
            return "NONE";
    }
}


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
            // case LCEVERSION::GENERATION:
            // return MCVERSION::MC_1_14;
            // case LCEVERSION::PISTON:
            // return MCVERSION::MC_1_14;
    }
}


MU static std::string mcVersionToString(const MCVERSION version) {
    switch (version) {
        case MC_1_0:
            return "1.0";
        case MC_1_1:
            return "1.1";
        case MC_1_2:
            return "1.2";
        case MC_1_3:
            return "1.3";
        case MC_1_4:
            return "1.4";
        case MC_1_5:
            return "1.5";
        case MC_1_6:
            return "1.6";
        case MC_1_7:
            return "1.7";
        case MC_1_8:
            return "1.8";
        case MC_1_9:
            return "1.9";
        case MC_1_10:
            return "1.10";
        case MC_1_11:
            return "1.11";
        case MC_1_12:
            return "1.12";
        case MC_1_13:
            return "1.13";
        case MC_1_14:
            return "1.14";
        default:
            return "NONE";
    }
}
