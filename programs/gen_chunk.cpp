
#include "LegacyCubiomes/chunk_generator/Chunk.hpp"
#include <fstream>

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "LegacyCubiomes/chunk_generator/biome.hpp"
#include "LegacyCubiomes/features/WorldGenerator/WorldGenLakes.hpp"
#include "LegacyCubiomes/structures/build/mineshaft.hpp"
#include "LegacyCubiomes/structures/build/stronghold.hpp"
#include "LegacyCubiomes/structures/build/village.hpp"
#include "LegacyCubiomes/structures/gen/village/village.hpp"
#include "LegacyCubiomes/structures/placement/StaticStructures.hpp"
#include "LegacyCubiomes/structures/placement/mineshaft.hpp"
#include "LegacyCubiomes/structures/placement/stronghold.hpp"
#include "LegacyCubiomes/structures/rolls/mineshaft.hpp"
#include "LegacyCubiomes/structures/rolls/stronghold.hpp"


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
    auto console = lce::CONSOLE::WIIU;
    auto version = LCEVERSION::ELYTRA;
    Generator g(console, version, -872420751, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);

    // 3 13 for seed -101, 8 15 for seed 1, 11 16 or 15 5 for seed 27184353441555
    int X_WIDTH = 27;
    int Z_WIDTH = 27;
    int X_CENTER = 0;
    int Z_CENTER = 0;

    BoundingBox worldBox;
    worldBox.minX = short(X_CENTER * 16 - X_WIDTH);
    worldBox.maxX = short(X_CENTER * 16 + X_WIDTH);
    worldBox.minY = 0;
    worldBox.minY = 256;
    worldBox.minZ = short(Z_CENTER * 16 - Z_WIDTH);
    worldBox.minZ = short(Z_CENTER * 16 + Z_WIDTH);



    auto world = World(&g);
    world.getOrCreateChunk({X_CENTER, Z_CENTER});
    world.decorateChunks({X_CENTER, Z_CENTER}, X_WIDTH);

    RNG rng;


    // Mineshaft
    if (true) {
        // get positions
        auto mineshaft_locations = Placement::Mineshaft::getAllPositions(g);

        // create them
        std::vector<gen::Mineshaft> valid_mineshafts = {};
        for (auto& pos : mineshaft_locations) {
            gen::Mineshaft mineshaft_gen = gen::Mineshaft();
            mineshaft_gen.generate(g.getConsole(), g.getWorldSeed(), pos.toChunkPos());
            valid_mineshafts.push_back(mineshaft_gen);
        }

        // build them
        for (auto& mineshaft_gen : valid_mineshafts) {

            int count = -1;
            std::cout << "\n\n";
            std::cout << "Mineshaft" << "\n";
            std::cout << "    Start: " << mineshaft_gen.startPos << "\n";
            std::cout << "    Pieces: " << mineshaft_gen.pieceArraySize << "\n\n";
            for (StructureComponent& piece: mineshaft_gen.pieceArray) {
                count += 1;

                if (piece.type == PieceType::PT_Mineshaft_NONE) break;
                std::cout << "    " << count << ". adding: " << gen::Mineshaft::getPieceName(piece.type) << piece;

                piece.structureType = static_cast<int>(mineshaft_gen.mineShaftType);
                bool result = build::mineshaft::addComponentParts(world, rng, mineshaft_gen.structureBB, piece);
                if (!result) {
                    std::cout << "    " << count << ". Piece::addComponentParts() failed.";
                }
                std::cout << "\n";
            }
        }
    }




    // Village
    std::cout << "\n\n";
    if (true) {

        // get positions
        auto village_locations = Placement::Village<false>::getAllPositions(&g);


        // create them
        std::vector<std::pair<Pos2D, gen::Village>> valid_villages = {};
        for (auto& village_pos : village_locations) {
            gen::Village village_gen(&g);
            village_gen.generate(village_pos.toChunkPos());
            valid_villages.emplace_back(village_pos, village_gen);
        }

        // build them
        for (auto& [village_pos, village_gen] : valid_villages) {
            int biomeType = build::village::getVillageBiomeType(g, village_pos);

            int count = -1;
            std::cout << "\n\n";
            std::cout << "Village" << "\n";
            std::cout << "    Start: " << village_pos << "\n";
            std::cout << "    Pieces: " << village_gen.pieceArraySize << "\n\n";
            for (auto& piece : village_gen.pieceArray) {
                count += 1;
                if (piece.type == PieceType::PT_Village_NONE) break;
                std::cout << "    " << count << ". adding: " << gen::Village::getPieceName(piece.type) << piece;
                piece.structureType = biomeType;
                bool result = build::village::addComponentParts(world, rng, village_gen.structureBB, piece);
                if (!result) {
                    std::cout << "    " << count << ". Piece::addComponentParts() failed.";
                }
                std::cout << "\n";
            }
        }
    }



    // Stronghold
    if (true) {
        int count;

        Pos2D strongholdPos = Placement::Stronghold::getWorldPosition(g).toChunkPos();
        auto stronghold_gen = gen::Stronghold();
        stronghold_gen.generate(g.getWorldSeed(), strongholdPos);

        /*
        count = 0;
        for (auto& piece : stronghold_gen.pieceArray) {
            count += 1;
            if (piece.type == PieceType::PT_Stronghold_NONE) break;

            Pos3D should_contain(121, 36, 59);
            if (piece.isVecInside(should_contain)) {
                std::cout << "    Piece #" << count << ": " << gen::Stronghold::getPieceName(piece.type) << piece << "\n";
                break;
            }
        }
        std::cout << "\n\n" << std::flush;
         */
        /*
        auto portalPiece = stronghold_gen.portalRoomPiece;
        Pos2D pos = {portalPiece->getWorldX(0, 0), portalPiece->getWorldZ(0, 0)};
        pos = pos.toChunkPos();
        BoundingBox boundingBox = BoundingBox::makeChunkBox(pos.x, pos.z);
        for (int x = boundingBox.minX; x <= boundingBox.maxX; x++) {
            for (int y = boundingBox.minY; y <= boundingBox.maxY; y++) {
                for (int z = boundingBox.minZ; z <= boundingBox.maxZ; z++) {
                    const auto* block = world.getBlock(x, y, z);
                    if (block->getID() == lce::blocks::ids::GRASS_ID) {
                        world.setBlock(x, y, z, lce::blocks::ids::DIAMOND_BLOCK_ID);
                    }
                }
            }
        }
        // BoundingBox areaBB = boundingBox;
         */

        count = -1;
        std::cout << "\n\n";
        std::cout << "Stronghold" << "\n";
        std::cout << "    Start: " << strongholdPos << "\n";
        std::cout << "    Pieces: " << stronghold_gen.pieceArraySize << "\n\n";
        for (auto& piece : stronghold_gen.pieceArray) {
            count += 1;
            if (piece.type == PieceType::PT_Stronghold_NONE) break;

            std::cout << "    " << count << ". adding: " << gen::Stronghold::getPieceName(piece.type) << piece;
            bool result = build::stronghold::addComponentParts(world, rng, stronghold_gen.structureBB, piece);
            if (!result) {
                std::cout << "    " << count << ". Piece::addComponentParts() failed.";
            }
            std::cout << "\n";
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
            // for (int i = 0; i < 16 * 16 * 256; i++) {
            //     if ((chunk->getBlockAtIndex(i) >> 4) == 1 && (chunk->getBlockAtIndex(i) & 15) == 0) {
            //         chunk->blocks[i] = 0;
            //     }
            // }

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

    // std::cout << "finished!\n";

    // file.close();

    // const char* cdPath = "cd \"C:\\Users\\jerrin\\CLionProjects\\LegacyChunkViewer\\build\"";
    // int result1 = system(cdPath);


    // const char* exePath = R"("C:\Users\jerrin\CLionProjects\LegacyChunkViewer\build\LegacyChunkViewer.exe")";

    // // Execute the executable
    // int result2 = system(exePath);

    // if (result2 == -1) {
    //     // Handle error
    //     std::cerr << "Failed to execute the program." << std::endl;
    //     return EXIT_FAILURE;
    // }

    // // Optional: Check the exit status of the executed program
    // std::cout << "Executed with return code: " << result2 << std::endl;

    // return EXIT_SUCCESS;

    return 0;
}