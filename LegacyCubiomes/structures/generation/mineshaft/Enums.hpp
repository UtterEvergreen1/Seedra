#pragma once

#include <map>
#include <string>



namespace mineshaft_generator {

    enum PieceType {
        ROOM = 0,
        CORRIDOR = 1,
        CROSSING = 2,
        STAIRS = 3,
        NONE,
    };

    enum class MineshaftType {
        NORMAL,
        MESA
    };

    const std::map<PieceType, std::string> PieceTypeName = {
            { PieceType::ROOM, "ROOM" },
            { PieceType::CORRIDOR, "CORRIDOR" },
            { PieceType::CROSSING, "CROSSING" },
            { PieceType::STAIRS, "STAIRS" },
            { PieceType::NONE, "NONE -> something went wrong" }
    };
}