#include <fstream>
#include <filesystem>
#include <cstdlib>

#include "terrain/Chunk.hpp"
#include "terrain/World.hpp"
#include "terrain/biomes/biome.hpp"

#include "structures/gen/village/village.hpp"
#include "structures/placement/StaticStructures.hpp"

namespace fs = std::filesystem;

int main() {
    const char* userProfile = std::getenv("USERPROFILE");
    if (!userProfile) {
        std::cerr << "USERPROFILE environment variable not set!\n";
        return 1;
    }
    fs::path dirPath = fs::path(userProfile)
                       / "CLionProjects"
                       / "LegacyChunkViewer";
    std::string dirName = dirPath.string();
    std::cout << "Using \"" + dirName + "\"...\n\n";
    fs::path filePath = dirPath / "build" / "chunks" / "chunkdata.bin";


    Biome::registerBiomes();
    auto console = lce::CONSOLE::WIIU;
    auto version = LCEVERSION::ELYTRA;
    // -6651998285536156346

    Generator g(console, version, 0, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);

    for (int i = 0; i < 10000; i++) {
        g.applyWorldSeed(i);

        auto world = World(&g);
        world.generateMineshafts();
        world.generateVillages();
        world.generateStrongholds();

        std::vector<int> stairDownIndexes;
        auto stronghold = world.strongholds[0];
        for (int pieceI = 0; pieceI < stronghold.getPieceCount(); pieceI++) {
            auto piece = stronghold.getPieceConst(pieceI);
            if (piece.type == PieceType::PT_Stronghold_StairsDown) {
                stairDownIndexes.emplace_back(pieceI);
            }
        }

        if (stairDownIndexes.size() != 1) {
            continue;
        }

        auto stairsDown = stronghold.getPieceConst(stairDownIndexes[0]);

        Pos2D center(stairsDown.getCenterX(), stairsDown.getCenterZ());

        auto chunkCenter = center.toChunkPos();

        int intersections = 0;
        for (int cx = -1; cx <= 1; cx++) {
            for (int cz = -1; cz <= 1; cz++) {
                auto chunkBB = BoundingBox::makeChunkBox(chunkCenter.x + cx, chunkCenter.z + cz);
                if (chunkBB.intersects(stairsDown)) {
                    intersections++;
                }
            }
        }

        if (intersections != 1) {
            continue;
        }

        std::cout << i << "\n";





    }









}









