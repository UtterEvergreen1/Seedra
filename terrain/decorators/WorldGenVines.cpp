#include "WorldGenVines.hpp"

#include "lce/blocks/__include.hpp"
#include "terrain/World.hpp"

using namespace lce::blocks;


bool BlockVine::canPlaceBlockOnSide(World* worldIn, const Pos3D& pos, const EnumFacing& facing) {
    if (facing == EnumFacing::UP || facing == EnumFacing::DOWN) { return false; }

    if (!canBlockStay(worldIn->getBlockId(pos.offset(getOppositeFacing(facing))))) { return false; }

    const int blockId = worldIn->getBlockId(pos.up());
    if (blockId != AIR_ID && blockId != VINES_ID && !canBlockStay(blockId)) { return false; }

    return true;
}

bool BlockVine::canBlockStay(const int blockId) {
    if ((blockId >= WHITE_SHULKER_BOX_ID && blockId <= BLACK_GLAZED_TERRACOTTA_ID) || blockId == BEACON_ID ||
        blockId == CAULDRON_ID || blockId == GLASS_ID || blockId == STAINED_GLASS_ID || blockId == PISTON_ID ||
        blockId == STICKY_PISTON_ID || blockId == PISTON_HEAD_ID || blockId == WOODEN_TRAPDOOR_ID) {
        return false;
    }
    // only that face needs to be solid, but we'll just check if the block is solid
    return isSolidBlock(blockId);
}

bool WorldGenVines::generate(World* worldIn, RNG& rng, const Pos3D& position) const {
    Pos3D pos = position;

    while (pos.getY() < 128) {
        if (worldIn->isAirBlock(pos)) {
            for (const EnumFacing& facing: FACING_HORIZONTAL) {
                if (BlockVine::canPlaceBlockOnSide(worldIn, pos, facing)) {
                    worldIn->setBlockAndData(pos, VINES_ID, static_cast<u8>(states::Vine::withProperty(facing)));
                    break;
                }
            }
        } else {
            const int x_off = rng.nextInt(4) - rng.nextInt(4);
            const int z_off = rng.nextInt(4) - rng.nextInt(4);
            pos = pos.add(x_off, 0, z_off);
        }
        pos = pos.up();
    }

    return true;
}
