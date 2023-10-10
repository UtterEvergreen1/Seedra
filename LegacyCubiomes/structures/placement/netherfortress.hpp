#pragma once
#include "LegacyCubiomes/utils/rng.hpp"
#include "LegacyCubiomes/utils/Pos2D.hpp"

namespace Placement {
    class NetherFortress {
    public:
        ND static Pos2D getWorldPosition(int64_t worldSeed);
    };
}

