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
#include "structures/gen2/ResourceLocation.hpp"
#include "structures/placement/StaticStructures.hpp"


class World;
class Generator;
class ChunkPrimer;

namespace scattered_features {

    class MU Igloo : public ScatteredFeature {
        static const ResourceLocation IGLOO_TOP_ID;
        static const ResourceLocation IGLOO_MIDDLE_ID;
        static const ResourceLocation IGLOO_BOTTOM_ID;

    public:
        Igloo() = delete;
        MU Igloo(Generator* g, RNG& rng, c_int chunkX, c_int chunkZ)
            : ScatteredFeature(g, rng, chunkX, 64, chunkZ, 7, 5, 8) {}
        ~Igloo() override;
        bool addComponentParts(World& worldIn, MU RNG& rng, const BoundingBox& chunkBB) override;
    };



}
