#include "WorldGenDungeons.hpp"

#include "lce/blocks/__include.hpp"
#include "terrain/World.hpp"


std::string WorldGenDungeons::SPAWNER_TYPES[] = {"Skeleton", "Zombie", "Zombie", "Spider"};


bool WorldGenDungeons::generate(World *worldIn, RNG &rng, const Pos3D &position) const {
    using namespace lce::blocks;
    // Random dungeon dimensions
    const int halfSizeX = rng.nextInt(2) + 2;
    const int minX = -halfSizeX - 1;
    const int maxX = halfSizeX + 1;
    constexpr int minY = -1;
    constexpr int maxY = 4;
    const int halfSizeZ = rng.nextInt(2) + 2;
    const int minZ = -halfSizeZ - 1;
    const int maxZ = halfSizeZ + 1;
    int airCount = 0;

    // First pass: validate location has solid blocks on floor/ceiling and count airs
    for (int offsetX = minX; offsetX <= maxX; ++offsetX) {
        for (int offsetY = minY; offsetY <= maxY; ++offsetY) {
            for (int offsetZ = minZ; offsetZ <= maxZ; ++offsetZ) {
                Pos3D blockPos = position.add(offsetX, offsetY, offsetZ);
                const bool isSolid = lce::blocks::isSolidBlock(worldIn->getBlockId(blockPos));

                if (offsetY == minY && !isSolid) { return false; }

                if (offsetY == maxY && !isSolid) { return false; }

                if ((offsetX == minX || offsetX == maxX || offsetZ == minZ || offsetZ == maxZ) && offsetY == 0 &&
                    worldIn->isAirBlock(blockPos) &&
                    worldIn->isAirBlock(blockPos.up())) {
                    ++airCount;
                }
            }
        }
    }

    // Valid dungeon must have 1-5 air blocks
    if (airCount < 1 || airCount > 5) { return false; }

    // Second pass: build the dungeon structure
    for (int offsetX = minX; offsetX <= maxX; ++offsetX) {
        for (int offsetY = maxY - 1; offsetY >= minY; --offsetY) {
            for (int offsetZ = minZ; offsetZ <= maxZ; ++offsetZ) {
                Pos3D blockPos = position.add(offsetX, offsetY, offsetZ);

                // Check if this is an interior block (not on walls, floor, or ceiling)
                if (offsetX != minX && offsetY != minY && offsetZ != minZ &&
                    offsetX != maxX && offsetY != maxY && offsetZ != maxZ) {
                    if (worldIn->getBlockId(blockPos) != CHEST_ID) { worldIn->setBlockId(blockPos, AIR_ID); }
                } else if (blockPos.getY() >= 0 && !isSolidBlock(worldIn->getBlockId(blockPos.down()))) {
                    worldIn->setBlockId(blockPos, AIR_ID);
                } else if (isSolidBlock(worldIn->getBlockId(blockPos)) &&
                           worldIn->getBlockId(blockPos) != CHEST_ID) {
                    // Make floor mossy sometimes, walls are always cobblestone
                    if (offsetY == minY && rng.nextInt(4) != 0) {
                        worldIn->setBlockId(blockPos, MOSS_STONE_ID);
                    } else {
                        worldIn->setBlockId(blockPos, COBBLESTONE_ID);
                    }
                }
            }
        }
    }

    // Try to place up to 2 chests in the dungeon
    for (int chestAttempt = 0; chestAttempt < 2; ++chestAttempt) {
        for (int positionAttempt = 0; positionAttempt < 3; ++positionAttempt) {
            const int chestX = position.getX() + rng.nextInt(halfSizeX * 2 + 1) - halfSizeX;
            const int chestY = position.getY();
            const int chestZ = position.getZ() + rng.nextInt(halfSizeZ * 2 + 1) - halfSizeZ;
            Pos3D chestPos(chestX, chestY, chestZ);

            if (worldIn->isAirBlock(chestPos)) {
                int adjacentSolidWalls = 0;

                for (const auto facing: FACING_HORIZONTAL) {
                    if (isSolidBlock(worldIn->getBlockId(chestPos.offset(facing)))) { ++adjacentSolidWalls; }
                }

                // Place chest only if it has exactly one solid wall adjacent
                if (adjacentSolidWalls == 1) {
                    worldIn->setBlockId(chestPos, CHEST_ID);
                    rng.nextLong(); // skip the loot seed
                    /*TileEntity *tileentity1 = worldIn.getTileEntity(blockpos2);

                    if (auto chest = dynamic_cast<TileEntityChest*>(tileentity1)) {
                        chest->setLootTable(LootTableList::CHESTS_SIMPLE_DUNGEON, rng);
                    }*/

                    break;
                }
            }
        }
    }

    // Place mob spawner in the center
    worldIn->setBlockId(position, MONSTER_SPAWNER_ID);
    pickMobSpawner(rng);
    /*TileEntity *tileentity = worldIn.getTileEntity(position);

    if (auto spawner = dynamic_cast<TileEntityMobSpawner*>(tileentity)) {
        spawner->getSpawnerBaseLogic().setEntityId(pickMobSpawner(rng));
    } else {
        std::cerr << "Failed to fetch mob spawner entity at (" << position.getX() << ", " << position.getY() << ", " << position.getZ() << ")" << std::endl;
    }*/

    return true;
}

void WorldGenDungeons::pickMobSpawner(RNG &rng) { rng.nextInt(sizeof(SPAWNER_TYPES)); }
