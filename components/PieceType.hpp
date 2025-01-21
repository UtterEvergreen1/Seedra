#pragma once

#include "lce/processor.hpp"


enum PieceType : i8 {



    PT_Stronghold_NONE =  0,
    PT_Stronghold_Straight =  1,
    PT_Stronghold_PrisonHall =  2,
    PT_Stronghold_LeftTurn =  3,
    PT_Stronghold_RightTurn =  4,
    PT_Stronghold_RoomCrossing =  5,
    PT_Stronghold_StraightStairsDown =  6,
    PT_Stronghold_StairsDown =  7,
    PT_Stronghold_FiveCrossing =  8,
    PT_Stronghold_ChestCorridor =  9,
    PT_Stronghold_Library = 10,
    PT_Stronghold_PortalRoom = 11,
    PT_Stronghold_FillerCorridor = 12,


    PT_Mineshaft_NONE = 0,
    PT_Mineshaft_Room = 1,
    PT_Mineshaft_Corridor = 2,
    PT_Mineshaft_Crossing = 3,
    PT_Mineshaft_Stairs = 4,

    PT_NONE = 0,

    PT_Village_NONE =   0,
    PT_Village_Start =   1,
    PT_Village_Road =   2,
    PT_Village_House4Garden =   3,
    PT_Village_Church =   4,
    PT_Village_House1 =   5,
    PT_Village_WoodHut =   6,
    PT_Village_Hall =   7,
    PT_Village_Field1 =   8,
    PT_Village_Field2 =   9,
    PT_Village_House2 =  10,
    PT_Village_House3 =  11,
    PT_Village_Torch =  12,
};