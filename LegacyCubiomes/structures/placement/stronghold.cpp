#include "stronghold.hpp"
#include "LegacyCubiomes/utils/constants.hpp"
#include "LegacyCubiomes/cubiomes/rng.hpp"


namespace Placement {

    Pos2D Stronghold::getWorldPosition(const Generator& g){
        bool xboxStronghold = g.getConsole() == CONSOLE::XBOX;
        double dist, angle;
        int multiplier;
        uint64_t rng;
        Pos2D p{};
        int out;

        setSeed(&rng, g.getWorldSeed());

        angle = 2.0 * PI * nextDouble(&rng);

        for (int var7 = 0; var7 < 10; ++var7) {

            if (xboxStronghold)
                multiplier = nextInt(&rng, 4);
            dist = nextDouble(&rng) + 1.25;
            if (!xboxStronghold)
                multiplier = nextInt(&rng, 4);

            dist = dist * ((double)(multiplier) + 3.0);

            p.x = ((int)round(cos(angle) * dist) << 4) + 8;
            p.z = ((int)round(sin(angle) * dist) << 4) + 8;

            p = g.locateBiome(p.x, p.z, 112, stronghold_biomes, &rng, &out);

            if(out || (var7 == 9)) {
                return p;
            }

            angle += 2 * PI;
        }

        p.x = 0;
        p.z = 0;

        return p;
    }

    /**
     * This returns the non-pushed position of the stronghold, ignoring biome checks
     * @param worldSeed the world seed
     * @param xboxStronghold whether or not the console is Xbox.
     * @return stronghold position.
     */
    Pos2D Stronghold::getRawWorldPosition(int64_t worldSeed, bool xboxStronghold) {
        uint64_t rng;
        setSeed(&rng, worldSeed);
        double angle = 2.0 * PI * nextDouble(&rng);

        int multiplier;
        if (xboxStronghold)
            multiplier = nextInt(&rng, 4);

        double dist = nextDouble(&rng) + 1.25;

        if (!xboxStronghold)
            multiplier = nextInt(&rng, 4);

        dist *= ((double)(multiplier) + 3.0);
        return {((int)round(cos(angle) * dist) << 4) + 8,
                ((int)round(sin(angle) * dist) << 4) + 8};
    }

}
