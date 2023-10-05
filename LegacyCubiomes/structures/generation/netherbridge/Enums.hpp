#pragma once

#include <map>
#include <string>

#include "LegacyCubiomes/building_blocks/PieceWeight.hpp"

namespace netherbridge_generator {

    enum PieceType {
        CROSSING3,
        END,
        STRAIGHT,
        CORRIDOR3,
        CORRIDOR4,
        ENTRANCE,
        CROSSING2,
        CORRIDOR,
        CORRIDOR5,
        CORRIDOR2,
        NETHER_STALK_ROOM,
        THRONE,
        CROSSING,
        STAIRS,
        START,
        NONE
    };


    const std::map<PieceType, std::string> PieceTypeName = {
            {CROSSING3, "CROSSING3"},
            {END, "END"},
            {STRAIGHT, "STRAIGHT"},
            {CORRIDOR3, "CORRIDOR3"},
            {CORRIDOR4, "CORRIDOR4"},
            {ENTRANCE, "ENTRANCE"},
            {CROSSING2, "CROSSING2"},
            {CORRIDOR, "CORRIDOR"},
            {CORRIDOR5, "CORRIDOR5"},
            {CORRIDOR2, "CORRIDOR2"},
            {NETHER_STALK_ROOM, "NETHER_STALK_ROOM"},
            {THRONE, "THRONE"},
            {CROSSING, "CROSSING"},
            {STAIRS, "STAIRS"},
            {START, "START"},
            {NONE, "NONE -> something went wrong"},
    };
}