
#include "LegacyCubiomes/chunk_generator/Chunk.hpp"
#include <fstream>

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "LegacyCubiomes/chunk_generator/biome.hpp"
#include "LegacyCubiomes/features/WorldGenerator/WorldGenLakes.hpp"
#include "LegacyCubiomes/structures/build/mineshaft.hpp"
#include "LegacyCubiomes/structures/build/village.hpp"
#include "LegacyCubiomes/structures/generation/village/village.hpp"
#include "LegacyCubiomes/structures/placement/StaticStructures.hpp"
#include "LegacyCubiomes/structures/placement/mineshaft.hpp"
#include "LegacyCubiomes/structures/rolls/mineshaft.hpp"


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
    Generator g(console, version, 27184353441555, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);

    // 3 13 for seed -101, 8 15 for seed 1, 11 16 or 15 5 for seed 27184353441555
    int X_WIDTH = 8;
    int Z_WIDTH = 8;
    int X_CENTER = 0;
    int Z_CENTER = -25;


    auto world = World(&g);
    world.getOrCreateChunk({X_CENTER, Z_CENTER});
    world.decorateChunks({X_CENTER, Z_CENTER}, X_WIDTH);

    RNG rng;


    auto mineshaft_locations = Placement::Mineshaft::getAllPositions(g);
    // mineshaft_locations.emplace_back(0, 0);
    std::cout << "Mineshaft Positions this seed: \n";
    for (auto& pos : mineshaft_locations) {
        std::cout << pos << "\n";
    }
    std::cout << std::flush;

    for (auto& pos : mineshaft_locations) {

        gen::Mineshaft mineshaft_gen = gen::Mineshaft();
        mineshaft_gen.generate(g.getWorldSeed(), pos.toChunkPos());
        std::cout << "Start: " << mineshaft_gen.startX << " " << mineshaft_gen.startZ << "\n";
        std::cout << "Pieces: " << mineshaft_gen.pieceArraySize << "\n";


        for (auto piece: mineshaft_gen.pieceArray) {
            if (piece.type == PieceType::Mineshaft_NONE) break;

            std::cout << "adding: " << gen::Mineshaft::pieceTypeNames[piece.type] << piece << "\n";

            rolls::MineshaftPiece* obj = nullptr;
            switch (piece.type) {
                case PieceType::Mineshaft_Crossing:
                    obj = new rolls::Corridor(0);
                    break;
                case PieceType::Mineshaft_Room:
                    obj = new rolls::Room(0);
                    break;
                case PieceType::Mineshaft_Corridor:
                    obj = new rolls::Corridor(0);
                    break;
                case PieceType::Mineshaft_Stairs:
                    obj = new rolls::Stairs(0);
                    break;
                default:;
            }
            if (obj != nullptr) {
                memcpy(&obj->minX, &piece.minX, sizeof(piece));
                bool result = obj->addComponentParts(world, rng, mineshaft_gen.structureBB);
                // if (!result) { break; }
            }
        }
    }






    auto village_locations = Placement::Village<false>::getAllPositions(&g);
    village_locations.emplace_back(0, 0);
    std::cout << "Village Positions this seed:\n";
    for (auto& pos : village_locations) {
        std::cout << pos << "\n";
    }
    std::cout << std::flush;

    for (auto& pos : village_locations) {
        gen::Village village_gen(&g);
        village_gen.generate(pos.toChunkPos());

        int biomeType = 0;
        switch (g.getBiomeAt(1, pos)) {
            case BiomeID::plains:
                biomeType = 0;
                break;
            case BiomeID::desert:
                biomeType = 1;
                break;
            case BiomeID::savanna:
                biomeType = 2;
                break;
            case BiomeID::taiga:
                biomeType = 3;
                break;
        }

        for (auto piece : village_gen.pieceArray) {
            if (piece.type == PieceType::Village_NONE) break;

            std::cout << "adding: " << gen::Village::pieceTypeNames[piece.type] << piece << "\n";

            rolls::VillagePiece* obj = nullptr;
            switch (piece.type) {
                case PieceType::Village_House4Garden:
                    obj = new rolls::House4Garden(biomeType);
                    break;
                case PieceType::Village_Church:
                    obj = new rolls::Church(biomeType);
                    break;
                case PieceType::Village_House1:
                    obj = new rolls::House1(biomeType);
                    break;
                case PieceType::Village_Hall:
                    obj = new rolls::Hall(biomeType);
                    break;
                case PieceType::Village_Field1:
                    obj = new rolls::Field1(biomeType);
                    break;
                case PieceType::Village_Field2:
                    obj = new rolls::Field2(biomeType);
                    break;
                case PieceType::Village_House2:
                    obj = new rolls::House2(biomeType);
                    break;
                case PieceType::Village_House3:
                    obj = new rolls::House3(biomeType);
                    break;
                case PieceType::Village_Torch:
                    obj = new rolls::Torch(biomeType);
                    break;
                case PieceType::Village_WoodHut:
                    obj = new rolls::WoodHut(biomeType);
                    break;
                case PieceType::Village_Road:
                    obj = new rolls::Path(biomeType);
                    break;
                case PieceType::Village_Start:
                    obj = new rolls::Well(biomeType);
                    break;
                default:;
            }
            if (obj != nullptr) {
                memcpy(&obj->minX, &piece.minX, sizeof(Piece));
                bool result = obj->addComponentParts(world, rng, village_gen.structureBB);
                if (!result) {
                    break;
                }
            }
        }

    }



    std::string DIR_NAME = R"(C:/Users/Jerrin/CLionProjects/LegacyChunkViewer/)";
    std::string filename = DIR_NAME + R"(build/chunks/chunkdata.bin)";

    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return -1;
    }

    file.write(reinterpret_cast<const char*>(&X_CENTER), sizeof(X_CENTER));
    file.write(reinterpret_cast<const char*>(&Z_CENTER), sizeof(Z_CENTER));
    file.write(reinterpret_cast<const char*>(&X_WIDTH), sizeof(X_WIDTH));
    file.write(reinterpret_cast<const char*>(&Z_WIDTH), sizeof(Z_WIDTH));


    for (int cx = -X_WIDTH + X_CENTER; cx < X_WIDTH + X_CENTER - 1; cx++) {
        for (int cz = -Z_WIDTH + Z_CENTER; cz < Z_WIDTH + Z_CENTER - 1; cz++) {
            auto* chunk = world.getChunk({cx, cz});
            if (!chunk) {
                std::cerr << "Error getting chunk: " << cx << ", " << cz << std::endl;
                continue;
            }

            // replace all stone with air
            for (int i = 0; i < 16 * 16 * 256; i++) {
                if ((chunk->getBlockAtIndex(i) >> 4) == 1 && (chunk->getBlockAtIndex(i) & 15) == 0) {
                    chunk->blocks[i] = 0;
                }
            }

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