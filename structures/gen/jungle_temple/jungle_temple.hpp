

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

    class MU JunglePyramid : public ScatteredFeature {
        bool placedMainChest = false;
        bool placedHiddenChest = false;
        bool placedTrap1 = false;
        bool placedTrap2 = false;

    public:
        JunglePyramid() = default;
        MU JunglePyramid(Generator* g, RNG& rng, int chunkX, int chunkZ)
            : ScatteredFeature(g, rng, chunkX, 64, chunkZ, 12, 10, 15) {}
        ~JunglePyramid() override;
        bool addComponentParts(World& worldIn, MU RNG& rng, const BoundingBox& chunkBB) override;
    };

}
