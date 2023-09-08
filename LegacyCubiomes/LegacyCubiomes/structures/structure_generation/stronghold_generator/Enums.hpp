#pragma once

#include <map>
#include <string>

namespace stronghold_generator {
    enum class GenerationStep {
        PORTAL, // Generates the layout up to the portal room, Y level will be inaccurate
        LAYOUT, // Generates full layout, Y level will be inaccurate
        FULL,   // Generates full layout and calculates Y level for all pieces
    };

    enum class PieceType {
        STRAIGHT = 0,
        PRISON_HALL = 1,
        LEFT_TURN = 2,
        RIGHT_TURN = 3,
        ROOM_CROSSING = 4,
        STRAIGHT_STAIRS_DOWN = 5,
        STAIRS_DOWN = 6,
        FIVE_CROSSING = 7,
        CHEST_CORRIDOR = 8,
        LIBRARY = 9,
        PORTAL_ROOM = 10,
        FILLER_CORRIDOR = 11,
        NONE,
    };

    const std::map<PieceType, std::string> PieceTypeName = {
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
            { PieceType::FILLER_CORRIDOR, "FILLER_CORRIDOR" },
            { PieceType::NONE, "NONE -> something went wrong" }
    };
}