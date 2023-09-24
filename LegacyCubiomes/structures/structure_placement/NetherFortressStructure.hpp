#pragma once
#include "LegacyCubiomes/cubiomes/rng.hpp"
#include "LegacyCubiomes/utils/pos2D.hpp"

namespace Structure {
    class NetherFortressStructure {
        ND static Pos2D getWorldPosition(int64_t worldSeed);
    };
}

