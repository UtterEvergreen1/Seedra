#pragma once

#include <cstdint>
#include <string>

enum class DIMENSION : int8_t { NETHER = -1, HELL = -1, OVERWORLD = 0, END = 1 };

enum class CONSOLE : int8_t { XBOX = 0, PS3 = 1, WIIU = 2, PS4 = 3, NONE = 4 };

enum class LCEVERSION : int8_t {
    AQUATIC = 0,
    ELYTRA = 1,
    BOUNTIFUL = 2,
    HORSE = 3,
    POTIONS = 4,
    ADVENTURE = 5,
    // PISTON,
    // GENERATION, // not supported yet

    PS4_VP = 6,
    NONE = 7
};

/// Minecraft Java versions
enum MCVERSION : int8_t {
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

enum class DIRECTION : int8_t {
    NORTH = 0,
    SOUTH = 1,
    WEST = 2,
    EAST = 3,
};

enum class BIOMESCALE : int8_t { SMALL = 0, MEDIUM = 1, LARGE = 2 };

enum class WORLDSIZE : int8_t { CLASSIC = 0, SMALL = 1, MEDIUM = 2, LARGE = 3 };


enum class StructureType : uint8_t {
    DesertPyramid,
    JungleTemple,
    JunglePyramid = JungleTemple,
    SwampHut,
    Igloo,
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


inline static std::string LceVersionToString(LCEVERSION version) {
    switch (version) {
        case LCEVERSION::PS4_VP:
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

inline static int getChunkWorldBounds(WORLDSIZE worldSize) {
    switch (worldSize) {
        case WORLDSIZE::CLASSIC:
        default:
            return 27;
        case WORLDSIZE::SMALL:
            return 32;
        case WORLDSIZE::MEDIUM:
            return 96;
        case WORLDSIZE::LARGE:
            return 160;
    }
}


inline static std::string getStructureName(StructureType type) {
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

inline static std::string worldSizeToString(WORLDSIZE worldSize) {
    switch (worldSize) {
        case WORLDSIZE::CLASSIC:
        default:
            return "CLASSIC";
        case WORLDSIZE::SMALL:
            return "SMALL";
        case WORLDSIZE::MEDIUM:
            return "MEDIUM";
        case WORLDSIZE::LARGE:
            return "LARGE";
    }
}


inline static std::string biomeScaleToString(BIOMESCALE biomeScale) {
    switch (biomeScale) {
        case BIOMESCALE::SMALL:
            return "SMALL";
        case BIOMESCALE::MEDIUM:
            return "MEDIUM";
        case BIOMESCALE::LARGE:
            return "LARGE";
        default:
            return "NONE";
    }
}


inline static MCVERSION getMCVersion(LCEVERSION lceVersionIn) {
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
        case LCEVERSION::PS4_VP:
            return MCVERSION::MC_1_14;
            // case LCEVERSION::GENERATION:
            // return MCVERSION::MC_1_14;
            // case LCEVERSION::PISTON:
            // return MCVERSION::MC_1_14;
    }
}


inline std::string mcVersionToString(MCVERSION version) {
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
