#pragma once

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantFunctionResult"

#include "LegacyCubiomes/building_blocks/BoundingBox.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"

#include "LegacyCubiomes/building_blocks/StructureComponent.hpp"
#include "LegacyCubiomes/structures/gen/village/village.hpp"
#include "lce/blocks/blocks.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "LegacyCubiomes/cubiomes/BiomeID.hpp"
#include "lce/blocks/block_states.hpp"


namespace build::village {
    extern const lce::blocks::Block *getBiomeSpecificBlockState(const lce::blocks::Block *blockPtr, int structureType);

    MU ND extern int getAverageGroundLevel(World &worldIn, const BoundingBox &structureBB,
                                           const StructureComponent &piece);

    MU extern void func_189927_a(MU const World &p_189927_1_, gen::Village *villageIn, MU BoundingBox p_189927_2_,
                                 MU RNG &p_189927_3_, MU int p_189927_4_, MU int p_189927_5_, MU int p_189927_6_,
                                 MU enumFacing p_189927_7_);

    MU extern void func_189926_a(MU const World &p_189926_1_, gen::Village *villageIn, MU enumFacing p_189926_2_,
                                 MU int p_189926_3_, MU int p_189926_4_, MU int p_189926_5_,
                                 MU BoundingBox p_189926_6_);


    class MU Church final {
    public:
        Church() = delete;

        static bool addComponentParts(World &worldIn, MU RNG &rng, BoundingBox &structureBB, StructureComponent &piece);
    };

    class MU Field1 final {
    public:
        Field1() = delete;

        MU static const lce::blocks::Block *getRandomCropType(c_int data) {
            switch (data) {
                case 0:
                case 1:
                    return &lce::blocks::BlocksInit::CARROTS;
                case 2:
                case 3:
                    return &lce::blocks::BlocksInit::POTATOES;
                case 4:
                    return &lce::blocks::BlocksInit::BEETROOT_BLOCK;
                default:
                    return &lce::blocks::BlocksInit::WHEAT_CROPS;
            }
        }


        MU static int getMaxAgeFromCrop(c_int id) {
            switch (id) {
                case lce::blocks::ids::CARROTS_ID:
                case lce::blocks::ids::POTATOES_ID:
                case lce::blocks::ids::BEETROOT_BLOCK_ID:
                default:
                    return 4;
                case lce::blocks::ids::WHEAT_CROPS_ID:
                    return 8;
            }
        }

        static bool addComponentParts(World &worldIn, RNG &rng, BoundingBox &structureBB, StructureComponent &piece);
    };

    class MU Field2 final {
    public:
        Field2() = delete;

        MU static const lce::blocks::Block *getRandomCropType(const int data) {
            switch (data) {
                case 0:
                case 1:
                    return &lce::blocks::BlocksInit::CARROTS;
                case 2:
                case 3:
                    return &lce::blocks::BlocksInit::POTATOES;
                case 4:
                    return &lce::blocks::BlocksInit::BEETROOT_BLOCK;
                default:
                    return &lce::blocks::BlocksInit::WHEAT_CROPS;
            }
        }

        MU static int getMaxAgeFromCrop(c_int id) {
            switch (id) {
                case lce::blocks::ids::CARROTS_ID:
                case lce::blocks::ids::POTATOES_ID:
                case lce::blocks::ids::BEETROOT_BLOCK_ID:
                default:
                    return 4;
                case lce::blocks::ids::WHEAT_CROPS_ID:
                    return 8;
            }
        }

        static bool addComponentParts(World &worldIn, RNG &rng, BoundingBox &structureBB, StructureComponent &piece);
    };

    class MU Hall final {
    public:
        Hall() = delete;

        static bool addComponentParts(World &worldIn, MU RNG &rng, BoundingBox &structureBB, StructureComponent &piece);
    };

    class MU House1 final {
    public:
        House1() = delete;

        static bool addComponentParts(World &worldIn, MU RNG &rng, BoundingBox &structureBB, StructureComponent &piece);
    };

    class MU House2 final {
    public:
        House2() = delete;

        static bool addComponentParts(World &worldIn, MU RNG &rng, BoundingBox &structureBB, StructureComponent &piece);
    };

    class MU House3 final {
    public:
        House3() = delete;

        static bool addComponentParts(World &worldIn, MU RNG &rng, BoundingBox &structureBB, StructureComponent &piece);
    };

    class MU House4Garden final {
    public:
        House4Garden() = delete;

        static bool addComponentParts(World &worldIn, MU RNG &rng, BoundingBox &structureBB, StructureComponent &piece);
    };


    class MU Path final {
    public:
        Path() = delete;

        static bool addComponentParts(World &worldIn, MU RNG &rng, const BoundingBox &structureBB, const StructureComponent &piece);
    };


    class MU Torch final {
    public:
        Torch() = delete;

        static bool addComponentParts(World &worldIn, MU RNG &rng, BoundingBox &structureBB, StructureComponent &piece);
    };


    class MU WoodHut final {
    public:
        WoodHut() = delete;

        static bool addComponentParts(World &worldIn, MU RNG &rng, BoundingBox &structureBB, StructureComponent &piece);
    };


    class MU Well final {
    public:
        Well() = delete;

        static bool addComponentParts(World &worldIn, MU RNG &rng, BoundingBox &structureBB, StructureComponent &piece);
    };


    MU static int getVillageBiomeType(const Generator &g, const Pos2D &pos) {
        int biomeType = 0;
        switch (g.getBiomeAt(1, pos)) {
            case BiomeID::plains:
                biomeType = 0;
                break;
            case BiomeID::desert:
                biomeType = 1;
                break;
            case BiomeID::savanna:
                biomeType = 2;
                break;
            case BiomeID::taiga:
                biomeType = 3;
                break;
        }
        return biomeType;
    }


    MU static bool addComponentParts(
        World &worldIn, RNG &rng, BoundingBox &structureBB, StructureComponent &piece) {
        bool result = false;

        switch (piece.type) {
            case PT_Village_House4Garden:
                result = House4Garden::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Village_Church:
                result = Church::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Village_House1:
                result = House1::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Village_Hall:
                result = Hall::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Village_Field1:
                result = Field1::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Village_Field2:
                result = Field2::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Village_House2:
                result = House2::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Village_House3:
                result = House3::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Village_Torch:
                result = Torch::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Village_WoodHut:
                result = WoodHut::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Village_Road:
                result = Path::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            case PT_Village_Start:
                result = Well::addComponentParts(worldIn, rng, structureBB, piece);
                break;
            default: ;
        }

        return result;
    }
}; // namespace build::village


#pragma clang diagnostic pop
