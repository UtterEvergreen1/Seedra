#include "structures/gen/ScatteredFeature.hpp"

#include "components/StructureComponent.hpp"

#include "terrain/ChunkPrimer.hpp"
#include "terrain/World.hpp"

#include "lce/blocks/__include.hpp"

namespace scattered_features {

    using namespace lce::blocks;
    using namespace lce::blocks::states;


    bool Igloo::addComponentParts(World& worldIn, RNG& rng, const BoundingBox& chunkBB) {

        fillWithBlocks(worldIn, chunkBB, 0, 0, 0, scatteredFeatureSizeX, 100, scatteredFeatureSizeZ, lce::BlocksInit::DIAMOND_BLOCK.getDefaultState(), false);

        return true;
    }
}
