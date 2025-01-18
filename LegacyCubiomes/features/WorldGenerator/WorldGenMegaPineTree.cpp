#include "WorldGenMegaPineTree.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "LegacyCubiomes/utils/MathHelper.hpp"
#include "lce/blocks/block_ids.hpp"
#include "lce/blocks/blocks.hpp"

using namespace lce::blocks;


WorldGenMegaPineTree::WorldGenMegaPineTree(const bool useBaseHeight)
    : WorldGenHugeTrees(13, 15, &BlocksInit::SPRUCE_WOOD, &BlocksInit::SPRUCE_LEAVES), useBaseHeight(useBaseHeight) {}


bool WorldGenMegaPineTree::generate(World* worldIn, RNG& rand, const Pos3D& position) const {
    using namespace lce::blocks;
    c_int height = this->getHeight(rand);

    if (!ensureGrowable(worldIn, rand, position, height)) { return false; }

    this->createCrown(worldIn, position.getX(), position.getZ(), position.getY() + height, 0, rand);

    for (int j = 0; j < height; ++j) {
        Pos3D upPos = position.up(j);

        if (worldIn->isAirBlock(upPos) || ids::isLeavesBlock(worldIn->getBlockId(upPos))) {
            worldIn->setBlock(upPos, this->woodMetadata);
        }

        if (j < height - 1) {
            Pos3D addPos1 = position.add(1, j, 0);
            if (ids::isAirOrLeavesBlock(worldIn->getBlockId(addPos1))) {
                worldIn->setBlock(addPos1, this->woodMetadata);
            }

            Pos3D addPos2 = position.add(1, j, 1);
            if (ids::isAirOrLeavesBlock(worldIn->getBlockId(addPos2))) {
                worldIn->setBlock(addPos2, this->woodMetadata);
            }

            Pos3D addPos3 = position.add(0, j, 1);
            if (ids::isAirOrLeavesBlock(worldIn->getBlockId(addPos3))) {
                worldIn->setBlock(addPos3, this->woodMetadata);
            }
        }
    }

    return true;
}

void WorldGenMegaPineTree::createCrown(World* worldIn, c_int x, c_int z, c_int y, c_int val, RNG& rand) const {
    c_int i = rand.nextInt(5) + (this->useBaseHeight ? this->baseHeight : 3);
    int j = 0;

    for (int k = y - i; k <= y; ++k) {
        c_int l = y - k;
        c_int i1 = val + MathHelper::floor((float) l / (float) i * 3.5F);
        this->growLeavesLayerStrict(worldIn, Pos3D(x, k, z), i1 + (l > 0 && i1 == j && (k & 1) == 0 ? 1 : 0));
        j = i1;
    }
}

void WorldGenMegaPineTree::generateSaplings(World* worldIn, RNG& random, const Pos3D& pos) const {
    placePodzolCircle(worldIn, pos.west().north());
    placePodzolCircle(worldIn, pos.east(2).north());
    placePodzolCircle(worldIn, pos.west().south(2));
    placePodzolCircle(worldIn, pos.east(2).south(2));

    for (int i = 0; i < 5; ++i) {
        c_int j = random.nextInt(64);
        c_int k = j % 8;
        c_int l = j / 8;

        if (k == 0 || k == 7 || l == 0 || l == 7) { placePodzolCircle(worldIn, pos.add(-3 + k, 0, -3 + l)); }
    }
}

void WorldGenMegaPineTree::placePodzolCircle(World* worldIn, const Pos3D& center) {
    for (int i = -2; i <= 2; ++i) {
        for (int j = -2; j <= 2; ++j) {
            if (std::abs(i) != 2 || std::abs(j) != 2) { placePodzolAt(worldIn, center.add(i, 0, j)); }
        }
    }
}

void WorldGenMegaPineTree::placePodzolAt(World* worldIn, const Pos3D& pos) {
    for (int i = 2; i >= -3; --i) {
        Pos3D blockPos = pos.up(i);
        c_int blockId = worldIn->getBlockId(blockPos);

        if (blockId == ids::GRASS_ID || blockId == ids::DIRT_ID) {
            worldIn->setBlock(blockPos, &BlocksInit::PODZOL);
            break;
        }

        if (!worldIn->isAirBlock(blockPos) && i < 0) { break; }
    }
}