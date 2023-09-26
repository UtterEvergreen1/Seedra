#pragma once

enum class DIMENSION {
    NETHER   = -1,
    HELL     = -1,
    OVERWORLD = 0,
    END       = 1
};

enum class CONSOLE {
    XBOX = 0,
    PS3  = 1,
    WIIU = 2,
    PS4  = 3
};

enum class LCEVERSION
{
    //PISTON,
    //GENERATION, // not supported yet
    AQUATIC = 0,
    ELYTRA = 1,
    BOUNTIFUL = 2,
    HORSE = 3,
    POTIONS = 4,
    ADVENTURE = 5,

    PS4_VP = 6
};

/**
 * Minecraft Java versions
 */
enum MCversion
{
    MC_1_0, // <=1.0 Experimental!
    MC_1_1,  MC_1_2,  MC_1_3,  MC_1_4,  MC_1_5,  MC_1_6,
    MC_1_7,  MC_1_8,  MC_1_9,  MC_1_10, MC_1_11, MC_1_12,
    MC_1_13, MC_1_14
};

enum class DIRECTION {
    NORTH = 0,
    SOUTH = 1,
    WEST  = 2,
    EAST  = 3,
};

enum class BIOMESCALE {
    SMALL  = 0,
    MEDIUM = 1,
    LARGE  = 2
};

enum class WORLDSIZE {
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

inline static MCversion getMCVersion(LCEVERSION lceversion) {
    switch (lceversion) {
        case LCEVERSION::AQUATIC:
        default:
            return MCversion::MC_1_13;
        case LCEVERSION::ELYTRA:
            return MCversion::MC_1_12;
        case LCEVERSION::BOUNTIFUL:
            return MCversion::MC_1_8;
        case LCEVERSION::HORSE:
            return MCversion::MC_1_6;
        case LCEVERSION::POTIONS:
            return MCversion::MC_1_2;
        case LCEVERSION::ADVENTURE:
            return MCversion::MC_1_0;
        case LCEVERSION::PS4_VP:
            return MCversion::MC_1_14;
        //case LCEVERSION::GENERATION:
            //return MCversion::MC_1_14;
        //case LCEVERSION::PISTON:
            //return MCversion::MC_1_14;
    }
}

