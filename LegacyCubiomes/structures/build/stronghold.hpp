#pragma once

#include "LegacyCubiomes/building_blocks/StructureComponent.hpp"
#include "lce/blocks/block_states.hpp"
#include "lce/blocks/blocks.hpp"


namespace build::stronghold {
    enum class Door {
        OPENING = 1,
        WOOD_DOOR = 2,
        GRATES = 3,
        IRON_DOOR = 4
    };


    MU static Door getRandomDoor(c_int data) {
        switch (data) {
            case 0:
            case 1:
            default:
                return Door::OPENING;
            case 2:
                return Door::WOOD_DOOR;
            case 3:
                return Door::GRATES;
            case 4:
                return Door::IRON_DOOR;
        }
    }


    static void placeDoor(World &worldIn, MU RNG &rng, const BoundingBox &structureBB,
                          const StructureComponent &piece, const Door door, c_int x, c_int y, c_int z);


    class MU ChestCorridor final {
    public:
        MU ChestCorridor() = delete;

        static bool addComponentParts(
            World &worldIn, RNG &rng, const BoundingBox &structureBB, const StructureComponent &piece);
    };

    class Corridor final {
    public:
        MU Corridor() = delete;

        static bool addComponentParts(
            World &worldIn, MU RNG &rng, const BoundingBox &structureBB, const StructureComponent &piece);
    };

    class Crossing final {
    public:
        MU Crossing() = delete;

        static bool addComponentParts(
            World &worldIn, RNG &rng, const BoundingBox &structureBB, const StructureComponent &piece);
    };

    class MU LeftTurn final {
    public:
        MU LeftTurn() = delete;

        static bool addComponentParts(
            World &worldIn, RNG &rng, const BoundingBox &structureBB, const StructureComponent &piece);
    };

    class MU Library final {
    public:
        MU Library() = delete;

        static bool addComponentParts(
            World &worldIn, RNG &rng, const BoundingBox &structureBB, const StructureComponent &piece);
    };

    class MU PortalRoom final {
    public:
        MU PortalRoom() = delete;

        static bool addComponentParts(
            World &worldIn, RNG &rng, const BoundingBox &structureBB, const StructureComponent &piece);
    };

    class MU Prison final {
    public:
        MU Prison() = delete;


        static bool addComponentParts(
            World &worldIn, RNG &rng, const BoundingBox &structureBB, const StructureComponent &piece);
    };

    class MU RightTurn final {
    public:
        MU RightTurn() = delete;

        static bool addComponentParts(
            World &worldIn, RNG &rng, const BoundingBox &structureBB, const StructureComponent &piece);
    };

    class MU RoomCrossing final {
    public:
        MU RoomCrossing() = delete;

        static bool addComponentParts(
            World &worldIn, RNG &rng, const BoundingBox &structureBB, const StructureComponent &piece);
    };

    class MU Stairs final {
    public:
        MU Stairs() = delete;

        static bool addComponentParts(
            World &worldIn, RNG &rng, const BoundingBox &structureBB, const StructureComponent &piece);
    };


    class MU StairsStraight final {
    public:
        MU StairsStraight() = delete;

        static bool addComponentParts(
            World &worldIn, RNG &rng, const BoundingBox &structureBB, const StructureComponent &piece);
    };

    class MU Straight final {
    public:
        MU Straight() = delete;

        static bool addComponentParts(
            World &worldIn, RNG &rng, const BoundingBox &structureBB, const StructureComponent &piece);
    };


    MU static bool addComponentParts(
        World &worldIn, RNG &rng, const BoundingBox &structureBB, const StructureComponent &piece) {
        bool result = false;

        switch (piece.type) {
            case PT_Stronghold_Straight:
                result = Straight::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Stronghold_PrisonHall:
                result = Prison::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Stronghold_LeftTurn:
                result = LeftTurn::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Stronghold_RightTurn:
                result = RightTurn::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Stronghold_RoomCrossing:
                result = RoomCrossing::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Stronghold_StraightStairsDown:
                result = StairsStraight::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Stronghold_StairsDown:
                result = Stairs::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Stronghold_FiveCrossing:
                result = Crossing::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Stronghold_ChestCorridor:
                result = ChestCorridor::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Stronghold_Library:
                result = Library::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Stronghold_PortalRoom:
                result = PortalRoom::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Stronghold_FillerCorridor:
                result = Corridor::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            default: ;
        }
        return result;
    }
} // namespace build::stronghold
