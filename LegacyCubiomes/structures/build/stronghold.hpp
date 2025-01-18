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



    static void placeDoor(World& worldIn, MU RNG& rng, const BoundingBox& chunkBB,
                          const StructureComponent& piece, Door door, c_int x, c_int y, c_int z);



    class MU ChestCorridor final {
    public:
        MU ChestCorridor() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };

    class Corridor final {
    public:
        MU Corridor() = delete;

        static bool addComponentParts(
                World& worldIn, MU RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };

    class Crossing final {
    public:
        MU Crossing() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };

    class MU LeftTurn final {
    public:
        MU LeftTurn() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };

    class MU Library final {
    public:
        MU Library() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };

    class MU PortalRoom final {
    public:
        MU PortalRoom() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };

    class MU Prison final {
    public:
        MU Prison() = delete;


        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };

    class MU RightTurn final {
    public:
        MU RightTurn() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };

    class MU RoomCrossing final {
    public:
        MU RoomCrossing() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };

    class MU Stairs final {
    public:
        MU Stairs() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };


    class MU StairsStraight final {
    public:
        MU StairsStraight() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };

    class MU Straight final {
    public:
        MU Straight() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };



    MU extern bool addComponentParts(
            World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);


} // namespace build::stronghold