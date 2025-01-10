#pragma once

#include "lce/processor.hpp"


enum PieceType : i8 {
    NONE = 0,

    Village_NONE         =   0,
    Village_Start        =   1,
    Village_Road         =   2,
    Village_House4Garden =   3,
    Village_Church       =   4,
    Village_House1       =   5,
    Village_WoodHut      =   6,
    Village_Hall         =   7,
    Village_Field1       =   8,
    Village_Field2       =   9,
    Village_House2       =  10,
    Village_House3       =  11,
    Village_Torch        =  12,


    Stronghold_NONE               =  0,
    Stronghold_Straight           =  1,
    Stronghold_PrisonHall         =  2,
    Stronghold_LeftTurn           =  3,
    Stronghold_RightTurn          =  4,
    Stronghold_RoomCrossing       =  5,
    Stronghold_StraightStairsDown =  6,
    Stronghold_StairsDown         =  7,
    Stronghold_FiveCrossing       =  8,
    Stronghold_ChestCorridor      =  9,
    Stronghold_Library            = 10,
    Stronghold_PortalRoom         = 11,
    Stronghold_FillerCorridor     = 12,


    Mineshaft_NONE = 0,
    Mineshaft_Room = 1,
    Mineshaft_Corridor = 2,
    Mineshaft_Crossing = 3,
    Mineshaft_Stairs = 4,

};

