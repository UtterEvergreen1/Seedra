#include "terrain/generator.hpp"
#include <fstream>
#include <unordered_map>

#include "structures/placement/stronghold.hpp"

int main() {
    constexpr int64_t sampleSize = 10000000; // Number of seeds to sample
    Generator generator(lce::CONSOLE::XBOX360, LCEVERSION::AQUATIC, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);
    generator.generateCache(4);

    std::ofstream outFile("heat_map.txt");
    if (!outFile.is_open()) {
        std::cerr << "Failed to open heat_map.txt for writing.\n";
    }
    std::unordered_map <Pos2D, int, Pos2D::Hasher> chunkPositions;
    for (int seed = 0; seed < sampleSize; ++seed) {
        generator.applyWorldSeed(seed);
        Pos2D rawPos = Placement::Stronghold::getStartChunk(generator);
        chunkPositions[rawPos]++;
    }

    // Output the heat map to a file
    for (const auto& entry : chunkPositions) {
        outFile << entry.first.x << " " << entry.first.z << " " << static_cast<double>(entry.second) / (double)sampleSize << "\n";
    }
    outFile.close();
    std::cout << "Heat map saved to heat_map.txt\n";

    return 0;
}
