#include "WorldGenDungeons.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "lce/blocks/blocks.hpp"
#include "lce/blocks/block_ids.hpp"


std::string WorldGenDungeons::SPAWNERTYPES[] = {"Skeleton", "Zombie", "Zombie", "Spider"};


bool WorldGenDungeons::generate(World* worldIn, RNG& rng, const Pos3D& position) const {
    using namespace lce::blocks;
    // int i = 3;
    const int j = rng.nextInt(2) + 2;
    const int k = -j - 1;
    const int l = j + 1;
    // int i1 = -1;
    // int j1 = 4;
    const int k1 = rng.nextInt(2) + 2;
    const int l1 = -k1 - 1;
    const int i2 = k1 + 1;
    int j2 = 0;

    for (int k2 = k; k2 <= l; ++k2) {
        for (int l2 = -1; l2 <= 4; ++l2) {
            for (int i3 = l1; i3 <= i2; ++i3) {
                Pos3D blockpos = position.add(k2, l2, i3);
                const bool flag = ids::isSolidBlock(worldIn->getBlockId(blockpos));

                if (l2 == -1 && !flag) { return false; }

                if (l2 == 4 && !flag) { return false; }

                if ((k2 == k || k2 == l || i3 == l1 || i3 == i2) && l2 == 0 && worldIn->isAirBlock(blockpos) &&
                    worldIn->isAirBlock(blockpos.up())) {
                    ++j2;
                }
            }
        }
    }

    if (j2 >= 1 && j2 <= 5) {
        for (int k3 = k; k3 <= l; ++k3) {
            for (int i4 = 3; i4 >= -1; --i4) {
                for (int k4 = l1; k4 <= i2; ++k4) {
                    Pos3D blockpos1 = position.add(k3, i4, k4);

                    if (k3 != k && i4 != -1 && k4 != l1 && k3 != l && i4 != 4 && k4 != i2) {
                        if (worldIn->getBlockId(blockpos1) != ids::CHEST_ID) {
                            worldIn->setBlock(blockpos1, ids::AIR_ID);
                        }
                    } else if (blockpos1.getY() >= 0 && !ids::isSolidBlock(worldIn->getBlockId(blockpos1.down()))) {
                        worldIn->setBlock(blockpos1, ids::AIR_ID);
                    } else if (ids::isSolidBlock(worldIn->getBlockId(blockpos1)) &&
                               worldIn->getBlockId(blockpos1) != ids::CHEST_ID) {
                        if (i4 == -1 && rng.nextInt(4) != 0) {
                            worldIn->setBlock(blockpos1, ids::MOSS_STONE_ID);
                        } else {
                            worldIn->setBlock(blockpos1, ids::COBBLESTONE_ID);
                        }
                    }
                }
            }
        }

        for (int l3 = 0; l3 < 2; ++l3) {
            for (int j4 = 0; j4 < 3; ++j4) {
                const int l4 = position.getX() + rng.nextInt(j * 2 + 1) - j;
                const int i5 = position.getY();
                const int j5 = position.getZ() + rng.nextInt(k1 * 2 + 1) - k1;
                Pos3D blockpos2(l4, i5, j5);

                if (worldIn->isAirBlock(blockpos2)) {
                    int j3 = 0;

                    for (const auto enumfacing: FACING_HORIZONTAL) {
                        if (ids::isSolidBlock(worldIn->getBlockId(blockpos2.offset(enumfacing)))) { ++j3; }
                    }

                    if (j3 == 1) {
                        worldIn->setBlock(blockpos2, ids::CHEST_ID);
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

        worldIn->setBlock(position, ids::MONSTER_SPAWNER_ID);
        pickMobSpawner(rng);
        /*TileEntity *tileentity = worldIn.getTileEntity(position);

        if (auto spawner = dynamic_cast<TileEntityMobSpawner*>(tileentity)) {
            spawner->getSpawnerBaseLogic().setEntityId(pickMobSpawner(rng));
        } else {
            std::cerr << "Failed to fetch mob spawner entity at (" << position.getX() << ", " << position.getY() << ", " << position.getZ() << ")" << std::endl;
        }*/

        return true;
    } else {
        return false;
    }
}

void WorldGenDungeons::pickMobSpawner(RNG& rng) { rng.nextInt(sizeof(SPAWNERTYPES)); }