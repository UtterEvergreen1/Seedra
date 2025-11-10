#pragma once

#include "components/StructureComponent.hpp"
#include "structures/gen/Structure.hpp"
#include "common/rng.hpp"

#include <map>
#include <vector>

#include "lce/blocks/block.hpp"
#include "lce/enums.hpp"
#include "structures/gen/FeaturePiece.hpp"
#include "structures/gen/ScatteredFeature.hpp"
#include "structures/placement/StaticStructures.hpp"


class World;
class Generator;
class ChunkPrimer;

namespace scattered_features {

    class MU Igloo : public ScatteredFeature {
    public:
        Igloo() = delete;
        MU Igloo(Generator* g, RNG& rng, int chunkX, int chunkZ)
            : ScatteredFeature(g, rng, chunkX, 64, chunkZ, 7, 5, 8) {}
        ~Igloo() = default;
        bool addComponentParts(World& worldIn, MU RNG& rng, const BoundingBox& chunkBB) override;
    };



}