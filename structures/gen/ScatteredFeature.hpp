#pragma once

#include "components/StructureComponent.hpp"
#include "structures/gen/Structure.hpp"
#include "common/rng.hpp"

#include <map>
#include <vector>

#include "lce/blocks/block.hpp"
#include "lce/enums.hpp"
#include "structures/placement/StaticStructures.hpp"


class World;
class Generator;
class ChunkPrimer;

namespace scattered_features {

    class ScatteredFeature : public StructureComponent {
    protected:
        int scatteredFeatureSizeX{};
        int scatteredFeatureSizeY{};
        int scatteredFeatureSizeZ{};
        int horizontalPos=-1;

        ScatteredFeature() {}

        ScatteredFeature(Generator* g, RNG& rng, int x, int y, int z, int sizeX, int sizeY, int sizeZ);

        bool offsetToAverageGroundLevel(World& worldIn, BoundingBox structureBB, int yOffset);

    public:
        virtual bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB) = 0;
    };


    class DesertPyramid : public ScatteredFeature {
        std::array<bool, 4> hasPlacedChest{};
    public:
        DesertPyramid() = default;
        DesertPyramid(Generator* g, RNG& rng, int chunkX, int chunkZ)
            : ScatteredFeature(g, rng, chunkX, 64, chunkZ, 21, 15, 21) {
        }
        ~DesertPyramid() = default;
        bool addComponentParts(
                World& worldIn, MU RNG& rng, const BoundingBox& chunkBB) override;
    };


    class JunglePyramid : public ScatteredFeature {
        bool placedMainChest;
        bool placedHiddenChest;
        bool placedTrap1;
        bool placedTrap2;
    public:
        JunglePyramid() = default;
        JunglePyramid(Generator* g, RNG& rng, int chunkX, int chunkZ)
            : ScatteredFeature(g, rng, chunkX, 64, chunkZ, 12, 10, 15) {
        }
        ~JunglePyramid() = default;
        bool addComponentParts(
                World& worldIn, MU RNG& rng, const BoundingBox& chunkBB) override;
    };



    class Igloo : public ScatteredFeature {
        std::array<bool, 4> hasPlacedChest{};
    public:
        Igloo() = default;
        Igloo(Generator* g, RNG& rng, int chunkX, int chunkZ)
            : ScatteredFeature(g, rng, chunkX, 64, chunkZ, 7, 5, 8) {
        }
        ~Igloo() = default;
        bool addComponentParts(
                World& worldIn, MU RNG& rng, const BoundingBox& chunkBB) override;
    };


    class SwampHut : public ScatteredFeature {
        std::array<bool, 4> hasPlacedChest{};
    public:
        SwampHut() = default;
        SwampHut(Generator* g, RNG& rng, int chunkX, int chunkZ)
            : ScatteredFeature(g, rng, chunkX, 64, chunkZ, 7, 7, 9) {
        }
        ~SwampHut() = default;
        bool addComponentParts(
                World& worldIn, MU RNG& rng, const BoundingBox& chunkBB) override;
    };


    ScatteredFeature* ScatteredFeatureFactory(
            Generator* g,
            const Placement::FeatureStructurePair& pair
    );


} // namespace scattered_features

