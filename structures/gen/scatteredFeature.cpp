#include "ScatteredFeature.hpp"

#include "components/StructureComponent.hpp"

#include "terrain/ChunkPrimer.hpp"
#include "terrain/World.hpp"

#include "lce/blocks/__include.hpp"

namespace scattered_features {

    using namespace lce::blocks;
    using namespace lce::blocks::states;


    ScatteredFeature::ScatteredFeature(Generator* g, RNG& rng, int x, int y, int z, int sizeX, int sizeY, int sizeZ)
        : StructureComponent() {
        this->scatteredFeatureSizeX = sizeX;
        this->scatteredFeatureSizeY = sizeY;
        this->scatteredFeatureSizeZ = sizeZ;
        EnumFacing direction = FACING_HORIZONTAL[rng.nextInt(4)];
        this->facing = direction;
        this->setCoordMode(direction);

        if (g->getLCEVersion() == LCEVERSION::AQUATIC) {
            if (direction == EnumFacing::NORTH) {
                setBoundingBox(BoundingBox(x, y, z - sizeZ, x + sizeX - 1, y + sizeY - 1, z));
            } else if (direction == EnumFacing::SOUTH) {
                setBoundingBox(BoundingBox(x, y, z, x + sizeX - 1, y + sizeY - 1, z + sizeZ - 1));
            } else {
                setBoundingBox(BoundingBox(x - sizeX, y, z - sizeZ, x, y + sizeY - 1, z));
            }
        } else {
            if (direction == EnumFacing::NORTH || direction == EnumFacing::SOUTH) {
                setBoundingBox(BoundingBox(x, y, z, x + sizeX - 1, y + sizeY - 1, z + sizeZ - 1)); // original
                //this.boundingBox = new StructureBoundingBox(x, y, z - sizeZ, x + sizeX - 1, y + sizeY - 1, z);
            } else {
                setBoundingBox(BoundingBox(x, y, z, x + sizeZ - 1, y + sizeY - 1, z + sizeX - 1)); // original
                //this.boundingBox = new StructureBoundingBox(x - sizeX, y, z - sizeZ, x, y + sizeY - 1, z);
            }
        }
    }


    bool ScatteredFeature::offsetToAverageGroundLevel(World& worldIn, BoundingBox structureBB, int yOffset) {
        if (this->horizontalPos >= 0) {
            return true;
        } else {
            int i = 0;
            int j = 0;
            Pos3D blockPos;

            for (int k=this->minZ; k <= this->maxZ; ++k) {
                for (int l=this->minX; l <= this->maxX; ++l) {
                    blockPos.setPos(l, 64, k);

                    if (structureBB.isVecInside(blockPos)) {
                        i += std::max(worldIn.getTopSolidOrLiquidBlock(blockPos).getY(),
                                    0); // worldIn.getAverageGroundLevel());
                        ++j;
                    }
                }
            }

            if (j == 0) {
                return false;
            } else {
                this->horizontalPos = i / j;
                this->offset(0, this->horizontalPos - this->minY + yOffset, 0);
                return true;
            }
        }
    }

    ScatteredFeature* ScatteredFeatureFactory(Generator* g, const Placement::FeatureStructurePair& pair) {
        RNG rng = RNG::getLargeFeatureSeed(g->getWorldSeed(), pair.pos.x, pair.pos.z);
        rng.advance();

        switch(pair.type) {
            case StructureType::DesertPyramid:
                return new DesertPyramid(g, rng, pair.pos.x, pair.pos.z);
            case StructureType::JunglePyramid:
                return new JunglePyramid(g, rng, pair.pos.x, pair.pos.z);
            case StructureType::SwampHut:
                return new SwampHut(g, rng, pair.pos.x, pair.pos.z);
            case StructureType::Igloo:
                return new Igloo(g, rng, pair.pos.x, pair.pos.z);
            default:
                return nullptr;
        }
    }




    // bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece) {
    //     bool result = DesertPyramid::addComponentParts(worldIn, rng, chunkBB, piece);
    //     return result;
    // }
}



