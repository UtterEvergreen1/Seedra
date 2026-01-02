#include "igloo.hpp"

#include "components/StructureComponent.hpp"

#include "terrain/ChunkPrimer.hpp"
#include "terrain/World.hpp"

#include "lce/blocks/__include.hpp"

namespace scattered_features {

    using namespace lce::blocks;
    using namespace lce::blocks::states;

    const ResourceLocation Igloo::IGLOO_TOP_ID = ResourceLocation("igloo/igloo_top");
    const ResourceLocation Igloo::IGLOO_MIDDLE_ID = ResourceLocation("igloo/igloo_middle");
    const ResourceLocation Igloo::IGLOO_BOTTOM_ID = ResourceLocation("igloo/igloo_bottom");

    FeaturePiece(IglooTop);
    FeaturePiece(IglooMiddle);
    FeaturePiece(IglooBottom);


    Igloo::~Igloo() = default;


    bool Igloo::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB) {
        if (!this->offsetToAverageGroundLevel(worldIn, chunkBB, -1)) {
            return false;
        } else {
            BoundingBox structureboundingbox= static_cast<BoundingBox>(*this);
            Pos3D blockpos(structureboundingbox.m_minX, structureboundingbox.m_minY, structureboundingbox.m_minZ);
            // auto arotation = Rotation::values();
            // MinecraftServer minecraftserver=worldIn.getMinecraftServer();
            DataFixer fixer;
            TemplateManager tm("structures", fixer);
            PlacementSettings ps = PlacementSettings()
                                           .setRotation(this->rotation)
                                           .setReplacedBlock(lce::BlocksInit::STRUCTURE_VOID.getDefaultState())
                                           .setBoundingBox(structureboundingbox);
            Template* _template= tm.getTemplate(IGLOO_TOP_ID);
            _template->addBlocksToWorldChunk(worldIn, blockpos, ps);

            if (rng.nextDouble() < 0.5) {
                Template* template1= tm.getTemplate(IGLOO_MIDDLE_ID);
                Template* template2= tm.getTemplate(IGLOO_BOTTOM_ID);
                int i=rng.nextInt(8) + 4;

                for (int j=0; j < i; ++j) {
                    Pos3D blockpos1=_template->calculateConnectedPos(ps, {3, -1 - j * 3, 5}, ps, {1, 2, 1});
                    template1->addBlocksToWorldChunk(worldIn, blockpos.add(blockpos1), ps);
                }

                Pos3D blockpos4=blockpos.add(_template->calculateConnectedPos(ps, {3, -1 - i * 3, 5}, ps, {3, 5, 7}));
                template2->addBlocksToWorldChunk(worldIn, blockpos4, ps);
                /*
                Map<BlockPos, String> map=template2.getDataBlocks(blockpos4, ps);

                for (Entry<BlockPos, String> entry : map.entrySet()) {
                    if ("chest".equals(entry.getValue())) {
                        Pos3D blockpos2=(BlockPos) entry.getKey();
                        worldIn.setBlockState(blockpos2, Blocks.AIR.getDefaultState(), 3);
                        TileEntity tileentity=worldIn.getTileEntity(blockpos2.down());

                        if (tileentity instanceof TileEntityChest) {
                            ((TileEntityChest) tileentity).setLootTable(LootTableList.CHESTS_IGLOO_CHEST, randomIn.nextLong());
                        }
                    }
                }
                 */
            } else {
                Pos3D blockpos3=Template::transformedBlockPos(ps, {3, 0, 5});
                worldIn.setBlock(blockpos.add(blockpos3), lce::BlocksInit::SNOW.getDefaultState()/*, 3*/);
            }

            return true;
        }
    }


    bool IglooTop::addComponentParts(World& worldIn, MU RNG& rng, const BoundingBox& chunkBB, StructureComponent& piece) {

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


    bool IglooMiddle::addComponentParts(World& worldIn, MU RNG& rng, const BoundingBox& chunkBB, StructureComponent& piece) {

        constexpr lce::BlockState snow = lce::BlocksInit::SNOW_BLOCK.getDefaultState();
        constexpr lce::BlockState carpet = lce::BlocksInit::RED_CARPET.getDefaultState();
        constexpr lce::BlockState ice = lce::BlocksInit::ICE.getDefaultState();
        constexpr lce::BlockState trapdoor = lce::BlocksInit::WOODEN_TRAPDOOR.getDefaultState();
        constexpr lce::BlockState bed = lce::BlocksInit::BED_BLOCK.getDefaultState();
        constexpr lce::BlockState crafting_table = lce::BlocksInit::CRAFTING_TABLE.getDefaultState();
        constexpr lce::BlockState redstone_torch = lce::BlocksInit::ON_REDSTONE_TORCH.getDefaultState();
        constexpr lce::BlockState furnace = lce::BlocksInit::FURNACE.getDefaultState();

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

        constexpr lce::BlockState snow = lce::BlocksInit::SNOW_BLOCK.getDefaultState();
        constexpr lce::BlockState carpet = lce::BlocksInit::RED_CARPET.getDefaultState();
        constexpr lce::BlockState ice = lce::BlocksInit::ICE.getDefaultState();
        constexpr lce::BlockState trapdoor = lce::BlocksInit::WOODEN_TRAPDOOR.getDefaultState();
        constexpr lce::BlockState bed = lce::BlocksInit::BED_BLOCK.getDefaultState();
        constexpr lce::BlockState crafting_table = lce::BlocksInit::CRAFTING_TABLE.getDefaultState();
        constexpr lce::BlockState redstone_torch = lce::BlocksInit::ON_REDSTONE_TORCH.getDefaultState();
        constexpr lce::BlockState furnace = lce::BlocksInit::FURNACE.getDefaultState();

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
