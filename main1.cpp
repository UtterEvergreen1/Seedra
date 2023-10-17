
#include <fstream>
#include <iostream>

#include "LegacyCubiomes/structures/include.hpp"
#include "LegacyCubiomes/enchants/include.hpp"
#include "LegacyCubiomes/loot/include.hpp"
#include "LegacyCubiomes/utils/include.hpp"
#include "LegacyCubiomes/cubiomes/include.hpp"

#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "LegacyCubiomes/chunk_generator/biome.hpp"


#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>

int main(int argc, char* argv[]) {
    Biome::registerBiomes();

    auto console = CONSOLE::WIIU;
    auto version = LCEVERSION::ELYTRA;
    EnchantmentHelper::setup(console, version);
    Generator g(console, version, 119, WORLDSIZE::CLASSIC, BIOMESCALE::SMALL);

    std::string path = R"(C:\Users\jerrin\CLionProjects\LegacyCubiomes\)";
    Picture pic = Picture(&g, path);
    pic.drawBiomes();

    // pic.drawBox(0, 0, 64, 64, 128, 255, 128);



    std::cout << "Village Positions" << std::endl;
    auto posVec = Placement::Village<false>::getAllPositions(&g);

    for (auto pos : posVec) {

        auto village = generation::Village(&g);
        village.generate(pos.toChunkPos());

        std::cout << "Village Pieces " << pos << std::endl;
        for (int i = 0; i < village.pieceArraySize; i++) {
            Piece* piece = &village.pieceArray[i];

            std::cout << village.pieceTypeNames.at(piece->type) << " ";
            std::cout << *piece << std::endl;
        }

        std::cout << std::endl;
        std::cout << "Has Blacksmith: " << (village.blackSmithPiece != nullptr) << std::endl;

        for (int i = 0; i < village.pieceArraySize; i++) {
            Piece* piece = &village.pieceArray[i];
            bool status =
                    pic.drawBox(108 + piece->minX / 4, 108 + piece->minZ / 4,
                                108 + piece->maxX / 4, 108 + piece->maxZ / 4,
                                0, 0, 0);
            if (!status)
                std::cout << "Failed to draw" << *piece << std::endl;
        }
    }

    pic.save();





    // g.applyWorldSeed(5887763652726489932);
    // auto pos = Placement::Stronghold::getWorldPosition(g);
    // std::cout << pos << std::endl;
}

/*
    // Placement::BuriedTreasure::setWorldSize(WORLDSIZE::CLASSIC);
    // writeBiomesToImage(&g, R"(C:\Users\jerrin\CLionProjects\LegacyCubiomes\)");
    // Pos2D pos = Placement::BuriedTreasure::getPosition(&g, 0, 0);
    // std::cout << g.getWorldSeed() << std::endl;
    // std::cout << pos << std::endl;
    // auto lol2 = Pos2D(-55 - 0, 389 - 16).toChunkPos();
    // Pos2D chunkPos = pos.toChunkPos();
    // auto* chunkPrimer = Chunk::provideChunk<true, false, false>(g, 0, 0);
     */

