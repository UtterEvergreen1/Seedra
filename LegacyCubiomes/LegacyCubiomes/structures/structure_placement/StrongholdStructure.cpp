#include "StrongholdStructure.hpp"
#include "LegacyCubiomes/utils/constants.hpp"
namespace Structure {
    char StrongholdStructure::VALID_BIOMES[ARRAY_SIZE];

    void StrongholdStructure::setup(){
        std::vector<char> biomeList = {
            plains, desert, extreme_hills, forest, taiga, hell, the_end, ice_plains, ice_mountains,
            mushroom_island, desert_hills, forest_hills, taiga_hills, extreme_hills_edge, jungle,
            jungle_hills, jungle_edge, stone_beach, birch_forest, birch_forest_hills, roofed_forest,
            cold_taiga, cold_taiga_hills, mega_taiga, mega_taiga_hills, extreme_hills_plus_trees,
            savanna, savanna_plateau, mesa, mesa_plateau_stone, mesa_plateau
        };

        for (unsigned int i = 0; i < biomeList.size(); i++)
            StrongholdStructure::VALID_BIOMES[(int)biomeList[i]] = 1;
    }

    Pos2D StrongholdStructure::getWorldPosition(Generator* g){
        double dist, angle;
        uint64_t rnds;
        Pos2D p{};
        int out;

        setSeed(&rnds, g->seed);

        angle = 2.0 * PI * nextDouble(&rnds);

        for (int var7 = 0; var7 < 10; ++var7)
        {
            int xboxStronghold = g->lceVersion >= XBOX_TU75 && g->lceVersion <= XBOX_TU75;

            int multiplier;
            if (xboxStronghold)
                multiplier = nextInt(&rnds, 4);
            dist = nextDouble(&rnds) + 1.25;
            if (!xboxStronghold)
                multiplier = nextInt(&rnds, 4);

            dist = dist * ((double)(multiplier) + 3.0);

            p.x = ((int)round(cos(angle) * dist) << 4) + 8;
            p.z = ((int)round(sin(angle) * dist) << 4) + 8;

            p = g->locateBiome(p.x, p.z, 112, VALID_BIOMES, &rnds, &out);

            if(out || (var7 == 9)) {
                p.x = p.x >> 4;
                p.z = p.z >> 4;
                return p;
            }

            angle += 2 * PI;
        }

        p.x = 0;
        p.z = 0;

        return p;
    }

    Pos2D StrongholdStructure::getRawWorldPosition(int64_t worldSeed, bool xboxStronghold){
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
