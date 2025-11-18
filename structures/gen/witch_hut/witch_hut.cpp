#include "structures/gen/ScatteredFeature.hpp"

#include "components/StructureComponent.hpp"

#include "terrain/ChunkPrimer.hpp"
#include "terrain/World.hpp"

#include "lce/blocks/__include.hpp"
#include "witch_hut.hpp"

namespace scattered_features {

    using namespace lce::blocks;
    using namespace lce::blocks::states;


    SwampHut::~SwampHut() = default;


    bool SwampHut::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB) {

        MU int sFSX = this->scatteredFeatureSizeX;
        MU int sFSZ = this->scatteredFeatureSizeZ;

        if (!this->offsetToAverageGroundLevel(worldIn, chunkBB, 0)) {
            return false;
        } else {
            this->fillWithBlocks(worldIn, chunkBB, 1, 1, 1, 5, 1, 7, lce::BlocksInit::SPRUCE_WOOD_PLANK.getDefaultState(), lce::BlocksInit::SPRUCE_WOOD_PLANK.getDefaultState(), false);
            this->fillWithBlocks(worldIn, chunkBB, 1, 4, 2, 5, 4, 7, lce::BlocksInit::SPRUCE_WOOD_PLANK.getDefaultState(), lce::BlocksInit::SPRUCE_WOOD_PLANK.getDefaultState(), false);
            this->fillWithBlocks(worldIn, chunkBB, 2, 1, 0, 4, 1, 0, lce::BlocksInit::SPRUCE_WOOD_PLANK.getDefaultState(), lce::BlocksInit::SPRUCE_WOOD_PLANK.getDefaultState(), false);
            this->fillWithBlocks(worldIn, chunkBB, 2, 2, 2, 3, 3, 2, lce::BlocksInit::SPRUCE_WOOD_PLANK.getDefaultState(), lce::BlocksInit::SPRUCE_WOOD_PLANK.getDefaultState(), false);
            this->fillWithBlocks(worldIn, chunkBB, 1, 2, 3, 1, 3, 6, lce::BlocksInit::SPRUCE_WOOD_PLANK.getDefaultState(), lce::BlocksInit::SPRUCE_WOOD_PLANK.getDefaultState(), false);
            this->fillWithBlocks(worldIn, chunkBB, 5, 2, 3, 5, 3, 6, lce::BlocksInit::SPRUCE_WOOD_PLANK.getDefaultState(), lce::BlocksInit::SPRUCE_WOOD_PLANK.getDefaultState(), false);
            this->fillWithBlocks(worldIn, chunkBB, 2, 2, 7, 4, 3, 7, lce::BlocksInit::SPRUCE_WOOD_PLANK.getDefaultState(), lce::BlocksInit::SPRUCE_WOOD_PLANK.getDefaultState(), false);
            this->fillWithBlocks(worldIn, chunkBB, 1, 0, 2, 1, 3, 2, lce::BlocksInit::OAK_WOOD.getDefaultState(), lce::BlocksInit::OAK_WOOD.getDefaultState(), false);
            this->fillWithBlocks(worldIn, chunkBB, 5, 0, 2, 5, 3, 2, lce::BlocksInit::OAK_WOOD.getDefaultState(), lce::BlocksInit::OAK_WOOD.getDefaultState(), false);
            this->fillWithBlocks(worldIn, chunkBB, 1, 0, 7, 1, 3, 7, lce::BlocksInit::OAK_WOOD.getDefaultState(), lce::BlocksInit::OAK_WOOD.getDefaultState(), false);
            this->fillWithBlocks(worldIn, chunkBB, 5, 0, 7, 5, 3, 7, lce::BlocksInit::OAK_WOOD.getDefaultState(), lce::BlocksInit::OAK_WOOD.getDefaultState(), false);
            this->setBlockState(worldIn, chunkBB, 2, 3, 2, lce::BlocksInit::OAK_FENCE.getDefaultState());
            this->setBlockState(worldIn, chunkBB, 3, 3, 7, lce::BlocksInit::OAK_FENCE.getDefaultState());
            this->setBlockState(worldIn, chunkBB, 1, 3, 4, lce::BlocksInit::AIR.getDefaultState());
            this->setBlockState(worldIn, chunkBB, 5, 3, 4, lce::BlocksInit::AIR.getDefaultState());
            this->setBlockState(worldIn, chunkBB, 5, 3, 5, lce::BlocksInit::AIR.getDefaultState());
            // this->setBlockState(worldIn, lce::BlocksInit::FLOWER_POT.getDefaultState().withProperty(BlockFlowerPot.CONTENTS, BlockFlowerPot.EnumFlowerType.MUSHROOM_RED), 1, 3, 5, chunkBB);
            this->setBlockState(worldIn, chunkBB, 3, 2, 6, lce::BlocksInit::CRAFTING_TABLE.getDefaultState());
            this->setBlockState(worldIn, chunkBB, 4, 2, 6, lce::BlocksInit::CAULDRON.getDefaultState());
            this->setBlockState(worldIn, chunkBB, 1, 2, 1, lce::BlocksInit::OAK_FENCE.getDefaultState());
            this->setBlockState(worldIn, chunkBB, 5, 2, 1, lce::BlocksInit::OAK_FENCE.getDefaultState());
            lce::BlockState iblockstate=lce::BlocksInit::SPRUCE_WOOD_STAIRS.getStateFromMeta(states::Stairs::withProperty(this->rotation.apply(this->mirror, EnumFacing::NORTH)));
            lce::BlockState iblockstate1=lce::BlocksInit::SPRUCE_WOOD_STAIRS.getStateFromMeta(states::Stairs::withProperty(this->rotation.apply(this->mirror, EnumFacing::EAST)));
            lce::BlockState iblockstate2=lce::BlocksInit::SPRUCE_WOOD_STAIRS.getStateFromMeta(states::Stairs::withProperty(this->rotation.apply(this->mirror, EnumFacing::WEST)));
            lce::BlockState iblockstate3=lce::BlocksInit::SPRUCE_WOOD_STAIRS.getStateFromMeta(states::Stairs::withProperty(this->rotation.apply(this->mirror, EnumFacing::SOUTH)));
            this->fillWithBlocks(worldIn, chunkBB, 0, 4, 1, 6, 4, 1, iblockstate, iblockstate, false);
            this->fillWithBlocks(worldIn, chunkBB, 0, 4, 2, 0, 4, 7, iblockstate1, iblockstate1, false);
            this->fillWithBlocks(worldIn, chunkBB, 6, 4, 2, 6, 4, 7, iblockstate2, iblockstate2, false);
            this->fillWithBlocks(worldIn, chunkBB, 0, 4, 8, 6, 4, 8, iblockstate3, iblockstate3, false);
            
            for (int i=2; i <= 7; i+=5) {
                for (int j=1; j <= 5; j+=4) {
                    this->replaceAirAndLiquidDownwards(worldIn, lce::BlocksInit::OAK_WOOD.getDefaultState(), j, -1, i, chunkBB);
                }
            }

            // if (!this->hasWitch) {
            //     int l=this->getXWithOffset(2, 5);
            //     int i1=this->getYWithOffset(2);
            //     int k=this->getZWithOffset(2, 5);
            //     if (chunkBB.isVecInside(new BlockPos(l, i1, k))) {
            //         this->hasWitch=true;
            //         EntityWitch entitywitch=new EntityWitch(worldIn);
            //         entitywitch.enablePersistence();
            //         entitywitch.setLocationAndAngles((double) l + 0.5D, (double) i1, (double) k + 0.5D, 0.0F, 0.0F);
            //         entitywitch.onInitialSpawn(worldIn.getDifficultyForLocation(new BlockPos(l, i1, k)), (IEntityLivingData) null);
            //         worldIn.spawnEntityInWorld(entitywitch);
            //     }
            // }

            return true;
        }
    }
}
