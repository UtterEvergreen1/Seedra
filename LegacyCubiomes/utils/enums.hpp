#pragma once

enum class DIMENSION : int8_t {
    NETHER   = -1,
    HELL     = -1,
    OVERWORLD = 0,
    END       = 1
};

enum class CONSOLE : int8_t {
    XBOX = 0,
    PS3  = 1,
    WIIU = 2,
    PS4  = 3,
    NONE = 4
};

enum class LCEVERSION : int8_t {
    AQUATIC = 0,
    ELYTRA = 1,
    BOUNTIFUL = 2,
    HORSE = 3,
    POTIONS = 4,
    ADVENTURE = 5,
    //PISTON,
    //GENERATION, // not supported yet

    PS4_VP = 6,
    NONE = 7
};

/// Minecraft Java versions
enum MCVERSION : int8_t {
    MC_1_0, // <=1.0 Experimental!
    MC_1_1,  MC_1_2,  MC_1_3,  MC_1_4,  MC_1_5,  MC_1_6,
    MC_1_7,  MC_1_8,  MC_1_9,  MC_1_10, MC_1_11, MC_1_12,
    MC_1_13, MC_1_14
};

enum class DIRECTION : int8_t {
    NORTH = 0,
    SOUTH = 1,
    WEST  = 2,
    EAST  = 3,
};

enum class BIOMESCALE : int8_t {
    SMALL  = 0,
    MEDIUM = 1,
    LARGE  = 2
};

enum class WORLDSIZE : int8_t {
    CLASSIC = 0,
    SMALL   = 1,
    MEDIUM  = 2,
    LARGE   = 3
};

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

