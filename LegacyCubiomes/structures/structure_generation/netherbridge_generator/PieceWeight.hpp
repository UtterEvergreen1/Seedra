#pragma once

namespace netherbridge_generator {

    struct PieceWeight {
        static const PieceWeight PIECE_WEIGHTS[15];

        int weight;
        int maxPlaceCount;
        int minDepth;
    };

    // gotten from __sti___16_UnityWorld10_cpp_34408de1
    inline const PieceWeight PieceWeight::PIECE_WEIGHTS[] = {
            {0x1e,  0, 1}, // BRIDGE_STRAIGHT = 0,
            {  10,  4, 0}, // NONE = 1
            {   0,  0, 0}, // BRIDGE_CROSSING = 2,
            {  10,  4, 0}, // ROOM_CROSSING = 3,
            {  10,  3, 0}, // STAIRS_ROOM = 4,
            { 0xf,  2, 0}, // MONSTER_THRONE = 5,
            { 0xf,  1, 0}, // CASTLE_ENTRANCE = 6,
            {0x1e,  2, 0}, // CASTLE_STALK_ROOM = 7,
            {0x19,  0, 1}, // CASTLE_SMALL_CORRIDOR_PIECE = 8,
            { 0xf,  5, 0}, // CASTLE_CORRIDOR_CROSSING_PIECE = 9,
            {   5, 10, 0}, // CASTLE_SMALL_CORRIDOR_RIGHT_TURN_PIECE = 0xa,
            {   5, 10, 0}, // CASTLE_SMALL_CORRIDOR_LEFT_TURN_PIECE = 0xb,
            {   3,  1, 0}, // CASTLE_CORRIDOR_STAIRS_PIECE = 0xc,
            {   7,  2, 0}, // CASTLE_CORRIDOR_BALCONY_PIECE = 0xd
    };
}













