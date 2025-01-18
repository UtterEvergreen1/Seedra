#pragma once

#include "lce/blocks/block_ids.hpp"
#include "lce/blocks/block_states.hpp"
#include "lce/blocks/blocks.hpp"
#include "LegacyCubiomes/utils/rng.hpp"
class StructureComponent;
class World;
class BoundingBox;

namespace build::mineshaft {


    MU ND static const lce::blocks::Block* getBiomeSpecificPlank(c_int structureType);


    MU ND static const lce::blocks::Block* getBiomeSpecificFence(c_int structureType);


    static bool func_189918_a(World& world, const BoundingBox& chunkBB, c_int minX, c_int maxX,
                              c_int y, c_int z, const StructureComponent& piece);


    class MU Room final {
    public:
        MU Room() = delete;

        static bool addComponentParts(
                World& worldIn, MU RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };


    class MU Corridor final {
    public:
        MU Corridor() = delete;

        static bool addComponentParts(
                World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);


        /// variable names may be incorrect
        static void func_189921_a(World& world, const BoundingBox& chunkBB,
            c_int minX, c_int minY, c_int minZ, c_int maxY, c_int maxX,
            RNG& rng, const StructureComponent& piece);

        static void placeCobwebsRandomly(World& world, const BoundingBox& chunkBB, RNG& rng,
            c_float chance, c_int x, c_int y, c_int z, const StructureComponent& piece);
    };


    class MU Crossing final {
    public:
        MU Crossing() = delete;
        
        static bool addComponentParts(
                World& worldIn, MU RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);


        static void placePlankPillar(World& world, const BoundingBox& chunkBB,
            c_int x, c_int minY, c_int z, c_int maxY, const StructureComponent& piece);
    };


    class MU Stairs final {
    public:
        MU Stairs() = delete;

        static bool addComponentParts(
                World& worldIn, MU RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);
    };


    MU extern bool addComponentParts(
            World& worldIn, RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);


} // namespace build::mineshaft