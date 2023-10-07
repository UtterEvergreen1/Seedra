#pragma once

#include <map>
#include <string>

#include "LegacyCubiomes/building_blocks/PieceWeight.hpp"

namespace stronghold_generator {

    enum class GenerationStep {
        PORTAL, // Generates the layout up to the portal room, Y level will be inaccurate
        LAYOUT, // Generates full layout, Y level will be inaccurate
        FULL,   // Generates full layout and calculates Y level for all pieces
    };

    enum PieceType {
        NONE,
        STRAIGHT,
        PRISON_HALL,
        LEFT_TURN,
        RIGHT_TURN,
        ROOM_CROSSING,
        STRAIGHT_STAIRS_DOWN,
        STAIRS_DOWN,
        FIVE_CROSSING,
        CHEST_CORRIDOR,
        LIBRARY,
        PORTAL_ROOM,
        FILLER_CORRIDOR,
    };


    const std::map<PieceType, std::string> PieceTypeName = {
            { PieceType::NONE, "NONE -> something went wrong" },
            { PieceType::STRAIGHT, "STRAIGHT" },
            { PieceType::PRISON_HALL, "PRISON_HALL" },
            { PieceType::LEFT_TURN, "LEFT_TURN" },
            { PieceType::RIGHT_TURN, "RIGHT_TURN" },
            { PieceType::ROOM_CROSSING, "ROOM_CROSSING" },
            { PieceType::STRAIGHT_STAIRS_DOWN, "STRAIGHT_STAIRS_DOWN" },
            { PieceType::STAIRS_DOWN, "STAIRS_DOWN" },
            { PieceType::FIVE_CROSSING, "FIVE_CROSSING" },
            { PieceType::CHEST_CORRIDOR, "CHEST_CORRIDOR" },
            { PieceType::LIBRARY, "LIBRARY" },
            { PieceType::PORTAL_ROOM, "PORTAL_ROOM" },
            { PieceType::FILLER_CORRIDOR, "FILLER_CORRIDOR" }
    };


}