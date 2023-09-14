#pragma once

enum DIMENSIONS {
    NETHER   = -1,
    HELL     = -1,
    OVERWORLD = 0,
    END       = 1
};

enum CONSOLES {
    XBOX = 0,
    PS3  = 1,
    WIIU = 1,
    PS4  = 2
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

