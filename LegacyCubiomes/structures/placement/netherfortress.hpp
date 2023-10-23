#pragma once
#include "LegacyCubiomes/utils/Pos2D.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

namespace Placement {
    class NetherFortress {
    public:
        ND static Pos2D getWorldPosition(int64_t worldSeed);
    };
} // namespace Placement
