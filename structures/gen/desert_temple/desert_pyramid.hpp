
#pragma once

#include "components/StructureComponent.hpp"
#include "structures/gen/ScatteredFeature.hpp"
#include "structures/placement/StaticStructures.hpp"


class World;
class Generator;
class ChunkPrimer;

namespace scattered_features {


    class DesertPyramid : public ScatteredFeature {
        MU std::array<bool, 4> hasPlacedChest{};

    public:
        DesertPyramid() = default;
        DesertPyramid(Generator* g, RNG& rng, c_int chunkX, c_int chunkZ)
            : ScatteredFeature(g, rng, chunkX, 64, chunkZ, 21, 15, 21) {}
        ~DesertPyramid() override;
        bool addComponentParts(World& worldIn, MU RNG& rng, const BoundingBox& chunkBB) override;
    };

}
