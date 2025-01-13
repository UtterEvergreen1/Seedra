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


    public:
        // enum class GenerationStep : i8 {
        //     VALID,      // Generates the layout up until it makes the village valid
        //     BLACKSMITH, // Generates the layout up to the blacksmith
        //     LAYOUT,     // Generates full layout
        //     FULL,       // Generates full layout and calculates hasMoreThanTwoComponents
        // };



        struct PieceWeight {
            MU PieceType pieceType;
            MU int weight;
            MU int PlaceCountMin;
            MU int PlaceCountMax;
        };

        MU static std::string PIECE_TYPE_NAMES[13];
        // static std::map<PieceType, std::string> PIECE_TYPE_NAMES;
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

        BoundingBox structureBB;

        StructureComponent pieceArray[ARRAY_SIZE];
        int pieceArraySize{};

        Pos2D startPos;




        explicit Village(const Generator* generator);

        /**
        * \n
        * Generates a mineshaft with the given chunk coordinates and stored generator.
        * @param chunkX x coordinate of the chunk
        * @param chunkZ z coordinate of the chunk
        */
        void generate(int chunkX, int chunkZ);

        /**
        * \n
        * Overload function. Generates a mineshaft with the given chunk coordinates and stored generator.
        * @param chunk coordinates of the chunk
        */
        void generate(const Pos2D chunk) { generate(chunk.x, chunk.z); }

    MU ND StructureComponent* getBlackSmithPiece() {
        if (myBlackSmithPieceIndex == -1) {
            return nullptr;
        }
        return &pieceArray[myBlackSmithPieceIndex];

    }
    MU ND bool hasMoreThanTwoComponents() const { return pieceArraySize - numInvalidPieces > 2; }

    MU static std::string getPieceName(PieceType pieceType);

    private:
        void setupPieces();
        ND int updatePieceWeight() const;
        static BoundingBox createPieceBB(PieceType pieceType, Pos3D pos, FACING direction);
        void buildComponentStart(const StructureComponent& piece);
        void buildComponent(const StructureComponent& scIn);
        BoundingBox road(Pos3D pos, FACING facing);
        void additionalRngRolls(Piece& p);
        StructureComponent generateComponent(Pos3D pos, FACING facing, i8 depth);
        StructureComponent genAndAddRoadPiece(Pos3D pos, FACING facing);
        StructureComponent genAndAddComponent(Pos3D pos, FACING facing, i8 depth);
        void addPiece(const StructureComponent& piece);
        ND bool hasCollisionPiece(const BoundingBox& boundingBox) const;
    };
} // namespace generation
