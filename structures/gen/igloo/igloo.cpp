#include "igloo.hpp"

#include "components/StructureComponent.hpp"

#include "terrain/ChunkPrimer.hpp"
#include "terrain/World.hpp"

#include "lce/blocks/__include.hpp"

namespace scattered_features {

    using namespace lce::blocks;
    using namespace lce::blocks::states;

    FeaturePiece(IglooTop);
    FeaturePiece(IglooMiddle);
    FeaturePiece(IglooBottom);




    bool Igloo::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB) {

        if (!this->offsetToAverageGroundLevel(worldIn, chunkBB, -1)) {
            return false;
        } else {
            Pos3D blockPos = Pos3D(this->minX, this->minY, this->minZ);

            StructureComponent piece(static_cast<const BoundingBox&>(*this), this->facing);
            std::cout << "Piece: " << piece << std::endl;
            IglooTop::addComponentParts(worldIn, rng, chunkBB, piece);

            if (true || rng.nextDouble() < 0.5) {
                int i = rng.nextInt(8) + 4;

                for (int j=0; j < i; ++j) {
                    // Pos3D blockpos1 = template.calculateConnectedPos(placementsettings, Pos3D(3, -1 - j * 3, 5),
                    //                                                  placementsettings, Pos3D(1, 2, 1));
                    // template1.addBlocksToWorldChunk(worldIn, blockPos.add(blockpos1), placementsettings);
                    IglooMiddle::addComponentParts(worldIn, rng, chunkBB, piece);
                }



                IglooBottom::addComponentParts(worldIn, rng, chunkBB, piece);
            } else {
                // Pos3D blockPos3 = Template.transformedBlockPos(placementsettings, Pos3D(3, 0, 5));
                // Pos3D blockPos3 =
                Pos3D sp = blockPos.add(3, 0, 5);
                setBlockState(worldIn, chunkBB, sp.x, sp.y, sp.z, lce::BlocksInit::SNOW_BLOCK.getDefaultState());
            }


            return true;
        }
    }


    bool IglooTop::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB, StructureComponent& piece) {

        const lce::BlockState snow = lce::BlocksInit::SNOW_BLOCK.getDefaultState();
        const lce::BlockState carpet = lce::BlocksInit::RED_CARPET.getDefaultState();
        const lce::BlockState ice = lce::BlocksInit::ICE.getDefaultState();
        const lce::BlockState trapdoor = lce::BlocksInit::WOODEN_TRAPDOOR.getDefaultState();
        const lce::BlockState bed = lce::BlocksInit::BED_BLOCK.getDefaultState();
        const lce::BlockState crafting_table = lce::BlocksInit::CRAFTING_TABLE.getDefaultState();
        const lce::BlockState redstone_torch = lce::BlocksInit::ON_REDSTONE_TORCH.getDefaultState();
        const lce::BlockState furnace = lce::BlocksInit::FURNACE.getDefaultState();

        // === Fill calls ===

        // -- Layer Y = 0 --
        piece.fillWithBlocks(worldIn, chunkBB, 2,0,0, 4,0,4, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1,0,2, 1,2,2, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 5,0,2, 5,2,2, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0,0,3, 1,0,5, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 5,0,3, 6,0,5, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 2,0,5, 2,0,7, snow,snow, false);
        piece.setBlockState(worldIn, chunkBB, 3,0,5, trapdoor);
        piece.fillWithBlocks(worldIn, chunkBB, 4,0,5, 4,0,7, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1,0,6, 1,2,6, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 3,0,6, 3,0,7, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 5,0,6, 5,2,6, snow,snow, false);

        // -- Layer Y = 1 --
        piece.fillWithBlocks(worldIn, chunkBB, 2,1,0, 2,2,1, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 4,1,0, 4,2,1, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0,1,3, 0,2,3, snow,snow, false);
        piece.setBlockState(worldIn, chunkBB, 1,1,3, furnace);
        piece.fillWithBlocks(worldIn, chunkBB, 2,1,3, 4,1,5, carpet,carpet, false);
        piece.fillWithBlocks(worldIn, chunkBB, 6,1,3, 6,2,3, snow,snow, false);
        piece.setBlockState(worldIn, chunkBB, 0,1,4, ice);
        piece.setBlockState(worldIn, chunkBB, 1,1,4, redstone_torch);
        piece.setBlockState(worldIn, chunkBB, 5,1,4, bed);
        piece.setBlockState(worldIn, chunkBB, 6,1,4, ice);
        piece.setBlockState(worldIn, chunkBB, 0,1,5, snow);
        piece.setBlockState(worldIn, chunkBB, 1,1,5, crafting_table);
        piece.setBlockState(worldIn, chunkBB, 5,1,5, bed);
        piece.setBlockState(worldIn, chunkBB, 6,1,5, snow);
        piece.fillWithBlocks(worldIn, chunkBB, 2,1,6, 4,1,6, carpet,carpet, false);
        piece.fillWithBlocks(worldIn, chunkBB, 2,1,7, 4,2,7, snow,snow, false);

        // -- Layer Y = 2 --
        piece.fillWithBlocks(worldIn, chunkBB, 0,2,4, 0,2,5, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 6,2,4, 6,2,5, snow,snow, false);

        // -- Layer Y = 3 --
        piece.fillWithBlocks(worldIn, chunkBB, 3,3,0, 3,3,2, snow,snow, false);
        piece.setBlockState(worldIn, chunkBB, 2, 3, 2, snow);
        piece.setBlockState(worldIn, chunkBB, 4, 3, 2, snow);
        piece.fillWithBlocks(worldIn, chunkBB, 1,3,3, 1,3,5, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 5,3,3, 5,3,5, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 2,3,6, 4,3,6, snow,snow, false);

        // -- Layer Y = 4 --
        piece.fillWithBlocks(worldIn, chunkBB, 2,4,3, 4,4,5, snow,snow, false);

        return true;
    }


    bool IglooMiddle::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB, StructureComponent& piece) {

        const lce::BlockState snow = lce::BlocksInit::SNOW_BLOCK.getDefaultState();
        const lce::BlockState carpet = lce::BlocksInit::RED_CARPET.getDefaultState();
        const lce::BlockState ice = lce::BlocksInit::ICE.getDefaultState();
        const lce::BlockState trapdoor = lce::BlocksInit::WOODEN_TRAPDOOR.getDefaultState();
        const lce::BlockState bed = lce::BlocksInit::BED_BLOCK.getDefaultState();
        const lce::BlockState crafting_table = lce::BlocksInit::CRAFTING_TABLE.getDefaultState();
        const lce::BlockState redstone_torch = lce::BlocksInit::ON_REDSTONE_TORCH.getDefaultState();
        const lce::BlockState furnace = lce::BlocksInit::FURNACE.getDefaultState();

        // === Fill calls ===

        // -- Layer Y = 0 --
        piece.fillWithBlocks(worldIn, chunkBB, 2,0,0, 4,0,4, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1,0,2, 1,2,2, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 5,0,2, 5,2,2, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0,0,3, 1,0,5, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 5,0,3, 6,0,5, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 2,0,5, 2,0,7, snow,snow, false);
        piece.setBlockState(worldIn, chunkBB, 3,0,5, trapdoor);
        piece.fillWithBlocks(worldIn, chunkBB, 4,0,5, 4,0,7, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1,0,6, 1,2,6, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 3,0,6, 3,0,7, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 5,0,6, 5,2,6, snow,snow, false);

        // -- Layer Y = 1 --
        piece.fillWithBlocks(worldIn, chunkBB, 2,1,0, 2,2,1, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 4,1,0, 4,2,1, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0,1,3, 0,2,3, snow,snow, false);
        piece.setBlockState(worldIn, chunkBB, 1,1,3, furnace);
        piece.fillWithBlocks(worldIn, chunkBB, 2,1,3, 4,1,5, carpet,carpet, false);
        piece.fillWithBlocks(worldIn, chunkBB, 6,1,3, 6,2,3, snow,snow, false);
        piece.setBlockState(worldIn, chunkBB, 0,1,4, ice);
        piece.setBlockState(worldIn, chunkBB, 1,1,4, redstone_torch);
        piece.setBlockState(worldIn, chunkBB, 5,1,4, bed);
        piece.setBlockState(worldIn, chunkBB, 6,1,4, ice);
        piece.setBlockState(worldIn, chunkBB, 0,1,5, snow);
        piece.setBlockState(worldIn, chunkBB, 1,1,5, crafting_table);
        piece.setBlockState(worldIn, chunkBB, 5,1,5, bed);
        piece.setBlockState(worldIn, chunkBB, 6,1,5, snow);
        piece.fillWithBlocks(worldIn, chunkBB, 2,1,6, 4,1,6, carpet,carpet, false);
        piece.fillWithBlocks(worldIn, chunkBB, 2,1,7, 4,2,7, snow,snow, false);

        // -- Layer Y = 2 --
        piece.fillWithBlocks(worldIn, chunkBB, 0,2,4, 0,2,5, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 6,2,4, 6,2,5, snow,snow, false);

        // -- Layer Y = 3 --
        piece.fillWithBlocks(worldIn, chunkBB, 3,3,0, 3,3,2, snow,snow, false);
        piece.setBlockState(worldIn, chunkBB, 2, 3, 2, snow);
        piece.setBlockState(worldIn, chunkBB, 4, 3, 2, snow);
        piece.fillWithBlocks(worldIn, chunkBB, 1,3,3, 1,3,5, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 5,3,3, 5,3,5, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 2,3,6, 4,3,6, snow,snow, false);

        // -- Layer Y = 4 --
        piece.fillWithBlocks(worldIn, chunkBB, 2,4,3, 4,4,5, snow,snow, false);

        return true;
    }


    bool IglooBottom::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB, StructureComponent& piece) {

        const lce::BlockState snow = lce::BlocksInit::SNOW_BLOCK.getDefaultState();
        const lce::BlockState carpet = lce::BlocksInit::RED_CARPET.getDefaultState();
        const lce::BlockState ice = lce::BlocksInit::ICE.getDefaultState();
        const lce::BlockState trapdoor = lce::BlocksInit::WOODEN_TRAPDOOR.getDefaultState();
        const lce::BlockState bed = lce::BlocksInit::BED_BLOCK.getDefaultState();
        const lce::BlockState crafting_table = lce::BlocksInit::CRAFTING_TABLE.getDefaultState();
        const lce::BlockState redstone_torch = lce::BlocksInit::ON_REDSTONE_TORCH.getDefaultState();
        const lce::BlockState furnace = lce::BlocksInit::FURNACE.getDefaultState();

        // === Fill calls ===

        // -- Layer Y = 0 --
        piece.fillWithBlocks(worldIn, chunkBB, 2,0,0, 4,0,4, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1,0,2, 1,2,2, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 5,0,2, 5,2,2, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0,0,3, 1,0,5, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 5,0,3, 6,0,5, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 2,0,5, 2,0,7, snow,snow, false);
        piece.setBlockState(worldIn, chunkBB, 3,0,5, trapdoor);
        piece.fillWithBlocks(worldIn, chunkBB, 4,0,5, 4,0,7, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 1,0,6, 1,2,6, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 3,0,6, 3,0,7, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 5,0,6, 5,2,6, snow,snow, false);

        // -- Layer Y = 1 --
        piece.fillWithBlocks(worldIn, chunkBB, 2,1,0, 2,2,1, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 4,1,0, 4,2,1, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 0,1,3, 0,2,3, snow,snow, false);
        piece.setBlockState(worldIn, chunkBB, 1,1,3, furnace);
        piece.fillWithBlocks(worldIn, chunkBB, 2,1,3, 4,1,5, carpet,carpet, false);
        piece.fillWithBlocks(worldIn, chunkBB, 6,1,3, 6,2,3, snow,snow, false);
        piece.setBlockState(worldIn, chunkBB, 0,1,4, ice);
        piece.setBlockState(worldIn, chunkBB, 1,1,4, redstone_torch);
        piece.setBlockState(worldIn, chunkBB, 5,1,4, bed);
        piece.setBlockState(worldIn, chunkBB, 6,1,4, ice);
        piece.setBlockState(worldIn, chunkBB, 0,1,5, snow);
        piece.setBlockState(worldIn, chunkBB, 1,1,5, crafting_table);
        piece.setBlockState(worldIn, chunkBB, 5,1,5, bed);
        piece.setBlockState(worldIn, chunkBB, 6,1,5, snow);
        piece.fillWithBlocks(worldIn, chunkBB, 2,1,6, 4,1,6, carpet,carpet, false);
        piece.fillWithBlocks(worldIn, chunkBB, 2,1,7, 4,2,7, snow,snow, false);

        // -- Layer Y = 2 --
        piece.fillWithBlocks(worldIn, chunkBB, 0,2,4, 0,2,5, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 6,2,4, 6,2,5, snow,snow, false);

        // -- Layer Y = 3 --
        piece.fillWithBlocks(worldIn, chunkBB, 3,3,0, 3,3,2, snow,snow, false);
        piece.setBlockState(worldIn, chunkBB, 2, 3, 2, snow);
        piece.setBlockState(worldIn, chunkBB, 4, 3, 2, snow);
        piece.fillWithBlocks(worldIn, chunkBB, 1,3,3, 1,3,5, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 5,3,3, 5,3,5, snow,snow, false);
        piece.fillWithBlocks(worldIn, chunkBB, 2,3,6, 4,3,6, snow,snow, false);

        // -- Layer Y = 4 --
        piece.fillWithBlocks(worldIn, chunkBB, 2,4,3, 4,4,5, snow,snow, false);

        return true;
    }


}
