#pragma once

#include "components/PieceWeight.hpp"
#include "lce/processor.hpp"

namespace gen {
    /**
     * @class NetherFortress
     * @brief Represents the Nether Fortress structure in the world generation system.
     *
     * This class defines the components, weights, and placement logic for generating
     * Nether Fortress structures in the game.
     */
    class NetherFortress {
    public:
        // #######################################################
        //              sub-structs and sub-classes
        // #######################################################

        /**
         * @enum PieceType
         * @brief Represents the different types of pieces used in the Nether Fortress structure.
         */
        enum PieceType {
            CROSSING3, ///< A 3-way crossing piece.
            END, ///< An end piece.
            STRAIGHT, ///< A straight corridor piece.
            CORRIDOR3, ///< A 3-way corridor piece.
            CORRIDOR4, ///< A 4-way corridor piece.
            ENTRANCE, ///< An entrance piece.
            CROSSING2, ///< A 2-way crossing piece.
            CORRIDOR, ///< A standard corridor piece.
            CORRIDOR5, ///< A 5-way corridor piece.
            CORRIDOR2, ///< A 2-way corridor piece.
            NETHER_STALK_ROOM, ///< A room containing Nether Stalks.
            THRONE, ///< A throne room piece.
            CROSSING, ///< A crossing piece.
            STAIRS, ///< A stairs piece.
            START, ///< The starting piece of the structure.
            NONE ///< No piece type.
        };

        /**
         * @var PIECE_WEIGHTS
         * @brief Defines the weights, maximum placement counts, and minimum depths for each piece type.
         */
        constexpr static PieceWeight PIECE_WEIGHTS[16] = {
            {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
            {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
        };

        /**
         * @struct PiecePlaceCount
         * @brief Tracks the placement count and constraints for a specific piece type.
         */
        struct PiecePlaceCount {
            PieceType pieceType; ///< The type of the piece.
            int placeCount; ///< The number of times this piece has been placed.

            /**
             * @brief Retrieves the weight configuration for a specific piece type.
             * @param pieceType The type of the piece.
             * @return A pointer to the corresponding PieceWeight.
             */
            ND static const PieceWeight *get(const PieceType pieceType) { return &PIECE_WEIGHTS[pieceType]; }

            /**
             * @brief Checks if the piece placement is valid based on its constraints.
             * @return True if the piece can be placed, false otherwise.
             */
            ND bool isValid() const {
                c_int maxPlaceCount = PIECE_WEIGHTS[pieceType].maxPlaceCount;
                return maxPlaceCount == 0 || placeCount < maxPlaceCount;
            }

            /**
             * @brief Determines if the piece can be placed at a specific depth.
             * @param depth The current depth in the structure hierarchy.
             * @return True if the piece can be placed, false otherwise.
             */
            ND bool canPlace(c_int depth) const { return isValid() && depth >= PIECE_WEIGHTS[pieceType].minDepth; }
        };

        /**
         * @var PIECE_PLACE_COUNT_DEFAULT
         * @brief Default placement counts for all piece types in the Nether Fortress.
         */
        const PiecePlaceCount PIECE_PLACE_COUNT_DEFAULT[16] = {
            {CROSSING3, 0}, {END, 0}, {STRAIGHT, 0}, {CORRIDOR3, 0}, {CORRIDOR4, 0}, {ENTRANCE, 0},
            {CROSSING2, 0}, {CORRIDOR, 0}, {CORRIDOR5, 0}, {CORRIDOR2, 0}, {NETHER_STALK_ROOM, 0}, {THRONE, 0},
            {CROSSING, 0}, {STAIRS, 0}, {START, 0}, {NONE, 0},
        };

        // #######################################################
        //       class attributes, variables, functions
        // #######################################################

        // ...
    };
} // namespace gen
