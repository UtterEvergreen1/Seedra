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
        virtual ~ScatteredFeature() = default;
        virtual bool addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB) = 0;
    };


    ScatteredFeature* ScatteredFeatureFactory(
            Generator* g,
            const Placement::FeatureStructurePair& pair
    );


} // namespace scattered_features

