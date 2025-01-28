#pragma once

#include "components/GenerationStep.hpp"
#include "components/StructureComponent.hpp"
#include "structures/gen/Structure.hpp"

#include <map>
#include <vector>


#include "lce/blocks/block.hpp"
#include "lce/enums.hpp"


class World;
class Generator;
class ChunkPrimer;




namespace gen {

    static constexpr u32 MINESHAFT_ARRAY_SIZE = 1024;

    class Mineshaft : public Structure<MINESHAFT_ARRAY_SIZE> {

        MU static std::string PIECE_TYPE_NAMES[5];


    public:
        StructureVariant myVariant = StructureVariant::SV_Mineshaft_Normal;


        /// debugging purposes
        // int collisionChecks = 0;

        ~Mineshaft();

        void generate(lce::CONSOLE console, i64 worldSeed, int chunkX, int chunkZ);
        void generate(lce::CONSOLE console, i64 worldSeed, Pos2D chunkPos);
        MU void reset();
        MU static std::string getPieceName(PieceType pieceType);

    private:
        bool collides(const BoundingBox& bbIn);
        StructureComponent* findCollisionPiece(const BoundingBox& bbIn);
        void genAndAddPiece(RNG& rng, Pos3D pos, EnumFacing facing, int depth);
        void buildComponent(RNG& rng, StructureComponent p);
    };

} // namespace gen



namespace build::mineshaft {


    MU ND extern lce::BlockState getBiomeSpecificPlank(c_int structureType);


    MU ND extern lce::BlockState getBiomeSpecificFence(c_int structureType);


    extern bool func_189918_a(World& world, const BoundingBox& chunkBB, c_int minX, c_int maxX,
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


namespace rolls {


    class MU Mineshaft {
    public:
        std::vector<std::pair<Pos3D, i64>> mineshaftChests{};

        void generateStructure(World& worldIn, const ChunkPrimer* chunk, const gen::Mineshaft* mg,
                               RNG& rng, int chunkX, int chunkZ);

        /// Generate all chests for a given mineshaft
        MU void generateAllChests(World& worldIn, const gen::Mineshaft* mg, const Generator& g, bool generateFullChunk);

        void generateChest(const ChunkPrimer* chunk, const BoundingBox& chunkBB,
                           const StructureComponent *piece, RNG& rng, int x, int y, int z);

    private:
        static void placeCobWeb(const ChunkPrimer* chunk, const BoundingBox& chunkBB,
                                const StructureComponent& piece, RNG& rng, int x, int z);
    };

}