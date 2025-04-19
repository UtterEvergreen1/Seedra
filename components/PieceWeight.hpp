#pragma once

#include "lce/processor.hpp"

/**
 * @struct PieceWeight
 * @brief Represents the weight and placement constraints for a structural piece.
 *
 * This structure is used to define the weight, maximum placement count, and minimum depth
 * for a structural piece in the generation process.
 */
struct PieceWeight {
    int weight; ///< The weight of the piece, used to determine its likelihood of being selected.
    int maxPlaceCount; ///< The maximum number of times this piece can be placed.
    int minDepth; ///< The minimum depth required for this piece to be placed.

    /**
     * @brief Gets the weight of the piece.
     * @return The weight of the piece.
     */
    ND int getWeight() const { return weight; }

    /**
     * @brief Gets the maximum placement count for the piece.
     * @return The maximum number of times this piece can be placed.
     */
    ND int getMaxCount() const { return maxPlaceCount; }

    /**
     * @brief Gets the minimum depth required for the piece.
     * @return The minimum depth required for this piece to be placed.
     */
    ND int getMinDepth() const { return minDepth; }
};