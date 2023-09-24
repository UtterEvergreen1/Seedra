#include "StrongholdStructure.hpp"
#include "LegacyCubiomes/utils/constants.hpp"

namespace Structure {
    Pos2D StrongholdStructure::getWorldPosition(const Generator& g){
        double dist, angle;
        int multiplier;
        uint64_t rnds;
        Pos2D p{};
        int out;

        setSeed(&rnds, g.getWorldSeed());

        angle = 2.0 * PI * nextDouble(&rnds);
        bool xboxStronghold = g.getConsole() == CONSOLE::XBOX;

        for (int var7 = 0; var7 < 10; ++var7)
        {;
            if (xboxStronghold)
                multiplier = nextInt(&rnds, 4);
            dist = nextDouble(&rnds) + 1.25;
            if (!xboxStronghold)
                multiplier = nextInt(&rnds, 4);

            dist = dist * ((double)(multiplier) + 3.0);

            p.x = ((int)round(cos(angle) * dist) << 4) + 8;
            p.z = ((int)round(sin(angle) * dist) << 4) + 8;

            p = g.locateBiome(p.x, p.z, 112, stronghold_biomes, &rnds, &out);

            if(out || (var7 == 9)) {
                return p;
            }

            angle += 2 * PI;
        }

        p.x = 0;
        p.z = 0;

        return p;
    }

    Pos2D StrongholdStructure::getRawWorldPosition(int64_t worldSeed, bool xboxStronghold) {
        uint64_t rnds;
        setSeed(&rnds, worldSeed);
        double angle = 2.0 * PI * nextDouble(&rnds);

        int multiplier;
        if (xboxStronghold)
            multiplier = nextInt(&rnds, 4);

        double dist = nextDouble(&rnds) + 1.25;

        if (!xboxStronghold)
            multiplier = nextInt(&rnds, 4);

        dist *= ((double)(multiplier)+3.0);
        return {((int)round(cos(angle) * dist) << 4) + 8, ((int)round(sin(angle) * dist) << 4) + 8};
    }
}
