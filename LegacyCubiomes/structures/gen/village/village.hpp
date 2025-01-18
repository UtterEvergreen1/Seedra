#pragma once

#include "LegacyCubiomes/building_blocks/GenerationStep.hpp"
#include "LegacyCubiomes/building_blocks/Piece.hpp"
#include "LegacyCubiomes/building_blocks/StructureComponent.hpp"
#include "LegacyCubiomes/cubiomes/layers.hpp"
#include "LegacyCubiomes/utils/Pos2DTemplate.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"
#include <map>
#include <vector>


namespace gen {

    class Village {
        static constexpr u32 ARRAY_SIZE = 512;

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

        int pendingRoadArray[ARRAY_SIZE]{};
        int pendingRoadArraySize{};


        const Generator* g;
        RNG rng;

        PieceType previousPiece{};

        GenerationStep generationStep = GS_Village_Full;

        /// number of ROADS + WELL
        int numInvalidPieces = 1;
        int myBlackSmithPieceIndex = -1;


    public:
        bool isZombieInfested{};

        int biomeType{};

        BoundingBox structureBB;

        StructureComponent pieceArray[ARRAY_SIZE];
        int pieceArraySize{};

        Pos2D startPos;

        Village() = delete;
        explicit Village(const Generator* generator);

        void generate(int chunkX, int chunkZ);
        void generate(Pos2D chunk);



    MU ND StructureComponent* getBlackSmithPiece();

    MU ND bool hasMoreThanTwoComponents() const { return pieceArraySize - numInvalidPieces > 2; }

    MU static std::string getPieceName(PieceType pieceType);

    private:

        MU void setVillageBiomeType();

        void setupPieces();
        ND int updatePieceWeight() const;
        static BoundingBox createPieceBB(PieceType pieceType, Pos3D pos, enumFacing direction);
        void buildComponentStart(const StructureComponent& piece);
        void buildComponent(const StructureComponent& scIn);
        BoundingBox road(Pos3D pos, enumFacing facing);
        void additionalRngRolls(Piece& p);
        StructureComponent generateComponent(Pos3D pos, enumFacing facing, i8 depth);
        StructureComponent genAndAddRoadPiece(Pos3D pos, enumFacing facing);
        StructureComponent genAndAddComponent(Pos3D pos, enumFacing facing, i8 depth);
        void addPiece(const StructureComponent& piece);
        ND bool hasCollisionPiece(const BoundingBox& boundingBox) const;
    };

} // namespace gen
