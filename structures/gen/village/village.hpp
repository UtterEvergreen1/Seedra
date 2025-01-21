#pragma once

#include "structures/gen/Structure.hpp"

#include "components/GenerationStep.hpp"
#include "components/StructureComponent.hpp"

#include "common/rng.hpp"

#include <map>
#include <vector>

class StructureComponent;
class Generator;
class World;

namespace gen {

    static constexpr u32 VILLAGE_ARRAY_SIZE = 256;

    class Village : public Structure<VILLAGE_ARRAY_SIZE> {

        struct PieceWeight {
            MU PieceType pieceType;
            MU int weight;
            MU int PlaceCountMin;
            MU int PlaceCountMax;
        };

        MU static std::string PIECE_TYPE_NAMES[13];

        static c_int VILLAGE_SIZE;
        static const PieceWeight PIECE_WEIGHTS[9];

    private:
        struct FinalPieceWeight {
            PieceType pieceType;
            int weight;
            int maxPlaceCount;
            int amountPlaced;
            FinalPieceWeight(const PieceType pieceType, c_int weight, c_int maxPlaceCount, c_int amountPlaced)
                : pieceType(pieceType), weight(weight), maxPlaceCount(maxPlaceCount), amountPlaced(amountPlaced) {}
        };

        std::vector<FinalPieceWeight> currentVillagePW;

        int pendingRoadArray[VILLAGE_ARRAY_SIZE]{};
        int pendingRoadArraySize{};


        const Generator* g;
        RNG rng;

        PieceType previousPiece{};

        GenerationStep generationStep = GS_Village_Full;

        /// number of ROADS + WELL
        int numInvalidPieces = 1;
        int myBlackSmithPieceIndex = -1;

        bool isZombieInfested{};


    public:
        StructureVariant myVariant{};



        Village() = delete;
        explicit Village(const Generator* generator);

        ~Village();
        void reset();

        void generate(int chunkX, int chunkZ);
        void generate(Pos2D chunk);



    MU ND const StructureComponent *getBlackSmithPiece();

    MU ND bool hasMoreThanTwoComponents() const { return pieceArraySize - numInvalidPieces > 2; }

    MU static std::string getPieceName(PieceType pieceType);

    private:

        MU void setVillageBiomeType();

        void setupPieces();
        ND int updatePieceWeight() const;
        static BoundingBox createPieceBB(PieceType pieceType, Pos3D pos, EnumFacing direction);
        void buildComponentStart(const StructureComponent& piece);
        void buildComponent(const StructureComponent& scIn);
        BoundingBox road(Pos3D pos, EnumFacing facing);
        void additionalRngRolls(StructureComponent& p);

        StructureComponent generateComponent(Pos3D pos, EnumFacing facing, i8 depth);

        void genAndAddRoadPiece(Pos3D pos, EnumFacing facing);

        StructureComponent genAndAddComponent(Pos3D pos, EnumFacing facing, i8 depth);
        void addPiece(StructureComponent& piece);
        ND bool hasCollisionPiece(const BoundingBox& boundingBox) const;
    };

} // namespace gen



namespace build::village {

    MU extern bool addComponentParts(
            World& worldIn, RNG& rng, BoundingBox& chunkBB, StructureComponent& piece);

    struct MU Path final {
        Path() = delete;
        static bool addComponentParts(
                World& worldIn, MU RNG& rng, BoundingBox& chunkBB, StructureComponent& piece);};

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
