
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
    auto version = LCEVERSION::AQUATIC;
    EnchantmentHelper::setup(console, version);
    Generator g(console, version, 119, WORLDSIZE::CLASSIC, BIOMESCALE::SMALL);

    std::string path = R"(C:\Users\jerrin\CLionProjects\LegacyCubiomes\)";
    // pic.drawBiomes();

    // pic.drawBox(0, 0, 64, 64, 128, 255, 128);

    WorldPicture pic(&g);
    pic.drawBiomesWithSize(7680, 7680);
    pic.save(path);

    std::cout << "DONE !!!" << std::endl;

    int x; std::cin >> x;



    std::cout << "Village Positions" << std::endl;
    auto posVec = Placement::Village<false>::getAllPositions(&g);
    Piece* piece;

    int count = -1;
    for (auto pos : posVec) {
        count++;
        auto village = generation::Village(&g);
        village.generate(pos.toChunkPos());

        int minX = INT32_MAX;
        int maxX = INT32_MIN;
        int minZ = INT32_MAX;
        int maxZ = INT32_MIN;

        std::cout << "Village Pieces " << pos << std::endl;
        for (int i = 0; i < village.pieceArraySize; i++) {
            piece = &village.pieceArray[i];
            if (piece->minX < minX) minX = piece->minX;
            if (piece->minZ < minZ) minZ = piece->minZ;
            if (piece->maxX > maxX) maxX = piece->maxX;
            if (piece->maxZ > maxZ) maxZ = piece->maxZ;
        }
        piece = village.blackSmithPiece;
        if (piece != nullptr) {
            if (piece->minX < minX) minX = piece->minX;
            if (piece->minZ < minZ) minZ = piece->minZ;
            if (piece->maxX > maxX) maxX = piece->maxX;
            if (piece->maxZ > maxZ) maxZ = piece->maxZ;
        }


        int width = maxX - minX;
        int height = maxZ - minZ;
        int size = std::max(width, height);

        Picture picture(size);
        std::cout << "Size " << size << std::endl;

        std::vector<unsigned char> rgb = {0, 0, 0};
        for (int i = 0; i < village.pieceArraySize; i++) {
            piece = &village.pieceArray[i];
            switch (piece->type) {
                using namespace generation;
                case Village::Church:
                    rgb = {255, 0, 0};  // Bright Red
                    break;
                case Village::WoodHut:
                    rgb = {205, 0, 0};  // Medium Red
                    break;
                case Village::Hall:
                    rgb = {155, 0, 0};  // Dark Red
                    break;
                case Village::Field1:
                    rgb = {0, 255, 0};  // Bright Green
                    break;
                case Village::Field2:
                    rgb = {0, 205, 0};  // Medium Green
                    break;
                case Village::House1:
                    rgb = {165, 42, 42};  // Brown
                    break;
                case Village::House2:
                    rgb = {0, 0, 255};  // Blacksmith
                    break;
                case Village::House3:
                    rgb = {210, 105, 30};  // Chocolate
                    break;
                case Village::House4Garden:
                    rgb = {205, 133, 63};  // Peru
                    break;
                case Village::Torch:
                    rgb = {255, 255, 0};  // Yellow
                    break;
                case Village::Start:
                    rgb = {128, 0, 255};  // Bright Purple
                    break;
                case Village::Road:
                    rgb = {105, 105, 105};  // Dim Gray
                    break;
            }

            bool status = picture.drawBox(
                piece->minX - minX,
                piece->minZ - minZ,
                piece->maxX - minX,
                piece->maxZ - minZ,
                rgb[0], rgb[1], rgb[2]
            );
            if (!status)
                std::cout << "failed to draw box" << std::endl;
        }

        piece = village.blackSmithPiece;
        if (piece != nullptr) {
            bool status = picture.drawBox(
                    piece->minX - minX,
                    piece->minZ - minZ,
                    piece->maxX - minX,
                    piece->maxZ - minZ,
                    0, 0, 255
            );
            if (!status)
                std::cout << "failed to draw box" << std::endl;
        }

        picture.saveWithName("village" + std::to_string(count) + "_" +
        std::to_string(pos.x) + "_" + std::to_string(pos.z) + ".png", path);

    }




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

