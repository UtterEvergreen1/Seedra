#pragma once

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantFunctionResult"

#include "LegacyCubiomes/building_blocks/BoundingBox.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"

#include "LegacyCubiomes/building_blocks/StructureComponent.hpp"
#include "LegacyCubiomes/structures/gen/village/village.hpp"
#include "lce/blocks/blocks.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "LegacyCubiomes/cubiomes/BiomeID.hpp"
#include "lce/blocks/block_states.hpp"


namespace build::village {

    MU extern bool addComponentParts(
            World& worldIn, RNG& rng, BoundingBox& chunkBB, StructureComponent& piece);

    struct MU Path final {
        Path() = delete;
        static bool addComponentParts(
                World& worldIn, MU RNG& rng, const BoundingBox& chunkBB, const StructureComponent& piece);};

    struct MU Church final {
        Church() = delete;
        static bool addComponentParts(
                World& worldIn, MU RNG& rng, BoundingBox& chunkBB, StructureComponent& piece);};

    struct MU Field1 final {
        Field1() = delete;
        static bool addComponentParts(
                World& worldIn, RNG& rng, BoundingBox& chunkBB, StructureComponent& piece);};

    struct MU Field2 final {
        Field2() = delete;
        static bool addComponentParts(
                World& worldIn, RNG& rng, BoundingBox& chunkBB, StructureComponent& piece);};

    struct MU Hall final {
        Hall() = delete;
        static bool addComponentParts(
                World& worldIn, MU RNG& rng, BoundingBox& chunkBB, StructureComponent& piece);};

    struct MU House1 final {
        House1() = delete;
        static bool addComponentParts(
                World& worldIn, MU RNG& rng, BoundingBox& chunkBB, StructureComponent& piece);};

    struct MU House2 final {
        House2() = delete;
        static bool addComponentParts(
                World& worldIn, MU RNG& rng, BoundingBox& chunkBB, StructureComponent& piece);};

    struct MU House3 final {
        House3() = delete;
        static bool addComponentParts(
                World& worldIn, MU RNG& rng, BoundingBox& chunkBB, StructureComponent& piece);};

    struct MU House4Garden final {
        House4Garden() = delete;
        static bool addComponentParts(
                World& worldIn, MU RNG& rng, BoundingBox& chunkBB, StructureComponent& piece);};

    struct MU Torch final {
        Torch() = delete;
        static bool addComponentParts(
                World& worldIn, MU RNG& rng, BoundingBox& chunkBB, StructureComponent& piece);};

    struct MU WoodHut final {
        WoodHut() = delete;
        static bool addComponentParts(
                World& worldIn, MU RNG& rng, BoundingBox& chunkBB, StructureComponent& piece);};

    struct MU Well final {
        Well() = delete;
        static bool addComponentParts(
                World& worldIn, MU RNG& rng, BoundingBox& chunkBB, StructureComponent& piece);};




}; // namespace build::village


#pragma clang diagnostic pop