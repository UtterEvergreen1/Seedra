
#include "LegacyCubiomes/chunk_generator/Chunk.hpp"
#include <fstream>

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "LegacyCubiomes/chunk_generator/biome.hpp"
#include "LegacyCubiomes/features/WorldGenerator/WorldGenLakes.hpp"
#include "LegacyCubiomes/structures/generation/village/village.hpp"
#include "LegacyCubiomes/structures/rolls/mineshaft.hpp"
#include "LegacyCubiomes/structures/rolls/village.hpp"


int main() {

    // rng tests
    /*RNG rngSeed;
    int64_t testLongSeed = 5093713735541695132LL;
    for (int i = 0; i < 100000; i++) {
        rngSeed = RNG::getPopulationSeed(-101, -9, 11);
        for (int x = 0; x < i; x++) {
            rngSeed.nextInt();
        }
        if (rngSeed.nextLong() == testLongSeed) {
            std::cout << i << " rolls" << std::endl;
            break;
        }
    }*/

    Biome::registerBiomes();
    auto console = lce::CONSOLE::XBOX360;
    auto version = LCEVERSION::ELYTRA;
    Generator g(console, version, 28379474629, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);

    // 3 13 for seed -101, 8 15 for seed 1, 11 16 or 15 5 for seed 27184353441555
    int WIDTH = 6;
    int X_OFF = 15;
    int Z_OFF = 5;


    std::string filename = R"(C:/Users/Jerrin/CLionProjects/LegacyChunkViewer/build/chunks/chunk_data_)"
                           + std::to_string(WIDTH) + ".bin";

    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return -1;
    }


    auto world = World(&g);
    world.getOrCreateChunk({X_OFF, Z_OFF});
    world.decorateChunks({X_OFF, Z_OFF}, WIDTH);

    generation::Village village(&g);
    village.generate(X_OFF, Z_OFF);

    RNG rng;

    for (auto piece : village.pieceArray) {
        if (piece.type == generation::Village::PieceType::NONE) break;

        std::cout << "adding: " << generation::Village::pieceTypeNames[piece.type] << piece << std::endl << "\n";

        switch (piece.type) {
            case generation::Village::PieceType::House4Garden: {
                auto obj = structure_rolls::House4Garden(&village, piece);
                obj.addComponentParts(world, rng, village.structureBoundingBox);
                break;
            }
            case generation::Village::PieceType::Church: {
                auto obj = structure_rolls::Church(&village, piece);
                obj.addComponentParts(world, rng, village.structureBoundingBox);
                break;
            }
            case generation::Village::PieceType::House1: {
                auto obj = structure_rolls::House1(&village, piece);
                obj.addComponentParts(world, rng, village.structureBoundingBox);
                break;
            }
            case generation::Village::PieceType::Hall: {
                auto obj = structure_rolls::Hall(&village, piece);
                obj.addComponentParts(world, rng, village.structureBoundingBox);
                break;
            }
            case generation::Village::PieceType::Field1: {
                auto obj = structure_rolls::Field1(&village, piece);
                obj.addComponentParts(world, rng, village.structureBoundingBox);
                break;
            }
            case generation::Village::PieceType::Field2: {
                auto obj = structure_rolls::Field2(&village, piece);
                obj.addComponentParts(world, rng, village.structureBoundingBox);
                break;
            }
            case generation::Village::PieceType::House2: {
                auto obj = structure_rolls::House2(&village, piece);
                obj.addComponentParts(world, rng, village.structureBoundingBox);
                break;
            }
            case generation::Village::PieceType::House3: {
                auto obj = structure_rolls::House3(&village, piece);
                obj.addComponentParts(world, rng, village.structureBoundingBox);
                break;
            }
            default:;
        }
    }




    for (int cx = -WIDTH + X_OFF; cx < WIDTH + X_OFF + 1; cx++) {
        for (int cz = -WIDTH + Z_OFF; cz < WIDTH + Z_OFF + 1; cz++) {
            auto* chunk = world.getChunk({cx, cz});
            if (!chunk) {
                std::cerr << "Error getting chunk: " << cx << ", " << cz << std::endl;
                continue;
            }

            //replace all stone with air
            /*for (int i = 0; i < 16 * 16 * 256; i++) {
                if ((chunk->getBlockAtIndex(i) >> 4) == 1 && (chunk->getBlockAtIndex(i) & 15) == 0) {
                    chunk->blocks[i] = 0;
                }
            }*/

            // useful for set certain blocks in the world
            /*
            //set all the blocks to all the blocks available
            lce::registry::BlockRegistry blocks;
            blocks.setup();
            int x = 0;
            int y = 0;
            int z = 0;
            for (auto block : blocks.REGISTRY.getAllValues()) {
                world->setBlock(x, y, z, block->object->getID(), block->object->getDataTag());
                x+=2;
                if (x == 16) {
                    x = 0;
                    z+=2;
                    if (z == 16) {
                        z = 0;
                        y+=2;
                    }
                }
            }*/



            file.write(reinterpret_cast<const char*>(&cx), sizeof(cx));
            file.write(reinterpret_cast<const char*>(&cz), sizeof(cz));

            file.write(reinterpret_cast<const char*>(chunk->blocks),
                       sizeof(chunk->blocks));

            if (!file) {
                std::cerr << "Error writing to file: " << filename << std::endl;
            }/* else {
                std::cout << "Chunk (" << cx << ", " << cz << ") "
                        "written to file: " << filename << std::endl;
            }*/
        }
    }

    std::cout << "finished!\n";

    file.close();


}