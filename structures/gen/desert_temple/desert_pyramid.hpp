
#pragma once

#include "components/StructureComponent.hpp"
#include "structures/gen/Structure.hpp"
#include "common/rng.hpp"

#include <map>
#include <vector>

#include "lce/blocks/block.hpp"
#include "lce/enums.hpp"
#include "structures/gen/ScatteredFeature.hpp"
#include "structures/placement/StaticStructures.hpp"


class World;
class Generator;
class ChunkPrimer;

namespace scattered_features {


    class DesertPyramid : public ScatteredFeature {
        std::array<bool, 4> hasPlacedChest{};

    public:
        DesertPyramid() = default;
        DesertPyramid(Generator* g, RNG& rng, int chunkX, int chunkZ)
            : ScatteredFeature(g, rng, chunkX, 64, chunkZ, 21, 15, 21) {}
        ~DesertPyramid() = default;
        bool addComponentParts(World& worldIn, MU RNG& rng, const BoundingBox& chunkBB) override;
    };

}