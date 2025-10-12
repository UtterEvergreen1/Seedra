#include "stronghold.hpp"
#include "common/constants.hpp"
#include "common/rng.hpp"


namespace Placement {
    bool Stronghold::useFarStronghold = false;

    std::vector<Pos2D> Stronghold::getWorldPositions(const Generator &g) {
        RNG rng;
        Pos2D p{};
        int out = 0;
        int multiplier = 0;
        c_bool xboxStronghold = lce::isXbox(g.getConsole());
        int maxAttempts = g.getConsole() == lce::CONSOLE::XBOX1 ? 30 : 10;
        std::vector<Pos2D> worldPositions(g.getStrongholdCount());
        rng.setSeed(g.getWorldSeed());

        double angle = static_cast<double>(TWO_PI_FLOAT) * rng.nextDouble();

        for (int attempts = 0; attempts < maxAttempts && !out; ++attempts) {
            for (int index = 0; index < g.getStrongholdCount(); ++index) {
                if (useFarStronghold) multiplier = 32;
                else if (xboxStronghold) multiplier = rng.nextInt(4) + 3;
                double dist = rng.nextDouble() + 1.25;
                if (!xboxStronghold && !useFarStronghold) multiplier = rng.nextInt(4) + 3;

                dist = dist * static_cast<double>(multiplier);

                p.x = (static_cast<int>(std::roundf(cos(angle) * dist)) << 4) + 8;
                p.z = (static_cast<int>(std::roundf(sin(angle) * dist)) << 4) + 8;

                p = g.locateBiome(p.x, p.z, 112, stronghold_biomes, rng, &out);
                if (out || attempts == maxAttempts - 1) worldPositions[index] = p.toChunkPos().toBlockPos() + 4;
                angle += static_cast<double>(TWO_PI_FLOAT) / g.getStrongholdCount();
            }

            // TODO: test other consoles
            if (g.getConsole() == lce::CONSOLE::XBOX1) {
                if (g.getWorldGenerator() == WORLDGENERATOR::DEFAULT)
                    angle = static_cast<double>(TWO_THIRDS_PI_FLOAT) * rng.nextDouble();
                else
                    angle = static_cast<double>(TWO_PI_FLOAT) * rng.nextDouble();
            }
        }

        return worldPositions;
    }
} // namespace Placement