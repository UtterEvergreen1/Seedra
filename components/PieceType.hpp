#pragma once

#include "lce/processor.hpp"

/**
 * @enum PieceType
 * @brief Represents the various types of structural pieces used in the generation of strongholds, mineshafts, and villages.
 *
 * This enumeration defines the different types of pieces that can be used in the generation
 * of structures, such as strongholds, mineshafts, and villages. Each type corresponds to a
 * specific structural component or feature.
 */
enum PieceType : i8 {
    // Stronghold piece types
    PT_Stronghold_NONE = 0, ///< No stronghold piece.
    PT_Stronghold_Straight = 1, ///< A straight corridor in a stronghold.
    PT_Stronghold_PrisonHall = 2, ///< A prison hall in a stronghold.
    PT_Stronghold_LeftTurn = 3, ///< A left turn in a stronghold corridor.
    PT_Stronghold_RightTurn = 4, ///< A right turn in a stronghold corridor.
    PT_Stronghold_RoomCrossing = 5, ///< A room crossing in a stronghold.
    PT_Stronghold_StraightStairsDown = 6, ///< Straight stairs going down in a stronghold.
    PT_Stronghold_StairsDown = 7, ///< Stairs going down in a stronghold.
    PT_Stronghold_FiveCrossing = 8, ///< A five-way crossing in a stronghold.
    PT_Stronghold_ChestCorridor = 9, ///< A corridor with a chest in a stronghold.
    PT_Stronghold_Library = 10, ///< A library in a stronghold.
    PT_Stronghold_PortalRoom = 11, ///< The portal room in a stronghold.
    PT_Stronghold_FillerCorridor = 12, ///< A filler corridor in a stronghold.

    // Mineshaft piece types
    PT_Mineshaft_NONE = 0, ///< No mineshaft piece.
    PT_Mineshaft_Room = 1, ///< A room in a mineshaft.
    PT_Mineshaft_Corridor = 2, ///< A corridor in a mineshaft.
    PT_Mineshaft_Crossing = 3, ///< A crossing in a mineshaft.
    PT_Mineshaft_Stairs = 4, ///< Stairs in a mineshaft.

    // General piece type
    PT_NONE = 0, ///< No piece type.

    // Village piece types
    PT_Village_NONE = 0, ///< No village piece.
    PT_Village_Start = 1, ///< The starting point of a village.
    PT_Village_Road = 2, ///< A road in a village.
    PT_Village_House4Garden = 3, ///< A house with a garden in a village.
    PT_Village_Church = 4, ///< A church in a village.
    PT_Village_House1 = 5, ///< A type 1 house in a village.
    PT_Village_WoodHut = 6, ///< A wooden hut in a village.
    PT_Village_Hall = 7, ///< A hall in a village.
    PT_Village_Field1 = 8, ///< A type 1 field in a village.
    PT_Village_Field2 = 9, ///< A type 2 field in a village.
    PT_Village_House2 = 10, ///< A type 2 house in a village.
    PT_Village_House3 = 11, ///< A type 3 house in a village.
    PT_Village_Torch = 12, ///< A torch in a village.
};
