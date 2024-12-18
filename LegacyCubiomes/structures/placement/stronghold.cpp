#include "stronghold.hpp"
#include "LegacyCubiomes/utils/constants.hpp"
#include "LegacyCubiomes/utils/rng.hpp"


namespace Placement {

    bool Stronghold::useFarStronghold = false;

    Pos2D Stronghold::getWorldPosition(const Generator& g) {
        c_bool xboxStronghold = g.getConsole() == lce::CONSOLE::XBOX360 ||
                                    g.getConsole() == lce::CONSOLE::XBOX1;
        int multiplier;
        RNG rng;
        Pos2D p{};
        int out;

        rng.setSeed(g.getWorldSeed());

        double angle = 2.0 * PI * rng.nextDouble();

        for (int var7 = 0; var7 < 10; ++var7) {

            if (useFarStronghold) multiplier = 32;
            else if (xboxStronghold) multiplier = rng.nextInt(4) + 3;
            double dist = rng.nextDouble() + 1.25;
            if (!xboxStronghold && !useFarStronghold) multiplier = rng.nextInt(4) + 3;

            dist = dist * ((double) multiplier);

            p.x = ((int) round(cos(angle) * dist) << 4) + 8;
            p.z = ((int) round(sin(angle) * dist) << 4) + 8;

            p = g.locateBiome(p.x, p.z, 112, stronghold_biomes, rng, &out);

            if (out || (var7 == 9)) { return p; }

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
    Pos2D Stronghold::getRawWorldPosition(i64 worldSeed, bool xboxStronghold) {
        RNG rng;
        rng.setSeed(worldSeed);
        c_double angle = 2.0 * PI * rng.nextDouble();

        int multiplier;
        if (xboxStronghold) multiplier = rng.nextInt(4);

        double dist = rng.nextDouble() + 1.25;

        if (!xboxStronghold) multiplier = rng.nextInt(4);

        dist *= (double) multiplier + 3.0;
        return {((int) round(cos(angle) * dist) << 4) + 8, ((int) round(sin(angle) * dist) << 4) + 8};
    }


} // namespace Placement
