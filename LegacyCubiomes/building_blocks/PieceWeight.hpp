#pragma once

#include "LegacyCubiomes/cubiomes/processor.hpp"

struct PieceWeight {
    int weight;
    int maxPlaceCount;
    int minDepth;

    ND inline int getWeight() const {
        return weight;
    }

    ND inline int getMaxCount() const {
        return maxPlaceCount;
    }

    ND int getMinDepth() const {
        return minDepth;
    }

};
