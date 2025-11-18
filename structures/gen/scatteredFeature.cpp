#include "ScatteredFeature.hpp"

#include "components/StructureComponent.hpp"

#include "terrain/World.hpp"

#include "lce/blocks/__include.hpp"

#include "structures/gen/desert_temple/desert_pyramid.hpp"
#include "structures/gen/igloo/igloo.hpp"
#include "structures/gen/jungle_temple/jungle_temple.hpp"
#include "structures/gen/witch_hut/witch_hut.hpp"

namespace scattered_features {

    using namespace lce::blocks;
    using namespace lce::blocks::states;


    ScatteredFeature::~ScatteredFeature() = default;


    ScatteredFeature::ScatteredFeature(Generator* g, RNG& rng, int chunkX, int y, int chunkZ, int sizeX, int sizeY, int sizeZ)
        : StructureComponent() {
        this->scatteredFeatureSizeX = sizeX;
        this->scatteredFeatureSizeY = sizeY;
        this->scatteredFeatureSizeZ = sizeZ;
        const EnumFacing direction = FACING_HORIZONTAL[rng.nextInt(4)];
        this->m_facing = direction;
        this->setCoordMode(direction);

        c_int x = chunkX << 4;
        c_int z = chunkZ << 4;

        if (g->getLCEVersion() == LCEVERSION::AQUATIC) {
            if (direction == EnumFacing::NORTH) {
                setBoundingBox(BoundingBox(
                    static_cast<bbType_t>(x),
                    static_cast<bbType_t>(y),
                    static_cast<bbType_t>(z - sizeZ),
                    static_cast<bbType_t>(x + sizeX - 1),
                    static_cast<bbType_t>(y + sizeY - 1),
                    static_cast<bbType_t>(z)
                ));
            } else if (direction == EnumFacing::SOUTH) {
                setBoundingBox(BoundingBox(
                    static_cast<bbType_t>(x),
                    static_cast<bbType_t>(y),
                    static_cast<bbType_t>(z),
                    static_cast<bbType_t>(x + sizeX - 1),
                    static_cast<bbType_t>(y + sizeY - 1),
                    static_cast<bbType_t>(z + sizeZ - 1)
                ));
            } else {
                setBoundingBox(BoundingBox(
                    static_cast<bbType_t>(x - sizeX),
                    static_cast<bbType_t>(y),
                    static_cast<bbType_t>(z - sizeZ),
                    static_cast<bbType_t>(x),
                    static_cast<bbType_t>(y + sizeY - 1),
                    static_cast<bbType_t>(z)
                ));
            }
        } else {
            if (direction == EnumFacing::NORTH || direction == EnumFacing::SOUTH) {
                setBoundingBox(BoundingBox(
                    static_cast<bbType_t>(x),
                    static_cast<bbType_t>(y),
                    static_cast<bbType_t>(z),
                    static_cast<bbType_t>(x + sizeX - 1),
                    static_cast<bbType_t>(y + sizeY - 1),
                    static_cast<bbType_t>(z + sizeZ - 1)
                ));
            } else {
                setBoundingBox(BoundingBox(
                    static_cast<bbType_t>(x),
                    static_cast<bbType_t>(y),
                    static_cast<bbType_t>(z),
                    static_cast<bbType_t>(x + sizeZ - 1),
                    static_cast<bbType_t>(y + sizeY - 1),
                    static_cast<bbType_t>(z + sizeX - 1)
                ));
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

            for (int k=this->m_minZ; k <= this->m_maxZ; ++k) {
                for (int l=this->m_minX; l <= this->m_maxX; ++l) {
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
                this->offset(0, this->horizontalPos - this->m_minY + yOffset, 0);
                return true;
            }
        }
    }

    ScatteredFeature* ScatteredFeatureFactory(Generator* g, const Placement::FeatureStructurePair& pair) {
        Pos2D structChunkPos = pair.m_pos.toChunkPos();
        RNG rng = RNG::getLargeFeatureSeed(g->getWorldSeed(), structChunkPos.x, structChunkPos.z);
        rng.advance();

        switch(pair.m_type) {
            case StructureType::DesertPyramid:
                return new DesertPyramid(g, rng, structChunkPos.x, structChunkPos.z);
            case StructureType::JunglePyramid:
                return new JunglePyramid(g, rng, structChunkPos.x, structChunkPos.z);
            case StructureType::SwampHut:
                return new SwampHut(g, rng, structChunkPos.x, structChunkPos.z);
            case StructureType::Igloo:
                return new Igloo(g, rng, structChunkPos.x, structChunkPos.z);

            case StructureType::FEATURE_NUM:
            case StructureType::Village:
            case StructureType::OceanRuin:
            case StructureType::Mansion:
            case StructureType::Monument:
            case StructureType::Treasure:
            case StructureType::Shipwreck:
            case StructureType::Outpost:
            case StructureType::Mineshaft:
            case StructureType::Fortress:
            case StructureType::EndCity:
            case StructureType::EndGateway:
            case StructureType::NONE:
                return nullptr;
            default:
                std::unreachable();
        }
    }




    // bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece) {
    //     bool result = DesertPyramid::addComponentParts(worldIn, rng, chunkBB, piece);
    //     return result;
    // }
}



