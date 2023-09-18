#pragma once

#include <map>
#include <string>

namespace mineshaft_generator {
    // There is no GeneratorStep?

    enum class PieceType {
        CORRIDOR = 0,
        CROSSING = 1,
        ROOM = 2,
        STAIRS = 3,
        NONE,
    };

    const std::map<PieceType, std::string> PieceTypeName = {
            { PieceType::CORRIDOR, "CORRIDOR" },
            { PieceType::CROSSING, "CROSSING" },
            { PieceType::ROOM, "ROOM" },
            { PieceType::STAIRS, "STAIRS" },
            { PieceType::NONE, "NONE -> something went wrong" }
    };
}