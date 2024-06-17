#pragma once
#include "LegacyCubiomes/utils/Pos2DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

namespace Placement {


    class NetherFortress {
    public:
        ND static Pos2D getWorldPosition(i64 worldSeed);
    };


}
