#include <fstream>
#include <algorithm>

#include "features/WorldGenerator/WorldGenLakes.hpp"
#include "terrain/Chunk.hpp"
#include "terrain/World.hpp"
#include "terrain/features/biome.hpp"

#include "structures/gen/village/village.hpp"

#include "structures/placement/StaticStructures.hpp"
#include "structures/placement/mineshaft.hpp"
#include "structures/placement/stronghold.hpp"
#include "support/timer.hpp"


void calculateStatistics(const std::vector<int>& data) {
    if (data.empty()) {
        std::cout << "No data available.\n";
        return;
    }

    // Copy and sort the data
    std::vector<int> sortedData = data;
    std::sort(sortedData.begin(), sortedData.end());

    int size = sortedData.size();

    // Calculate Range
    int range = sortedData.back() - sortedData.front();

    // Calculate Quartiles
    auto getPercentile = [](const std::vector<int>& sorted, float percentile) -> float {
        float index = percentile * (sorted.size() - 1);
        int lower = std::floor(index);
        int upper = std::ceil(index);

        if (lower == upper)
            return sorted[lower];

        return sorted[lower] + (index - lower) * (sorted[upper] - sorted[lower]);
    };

    float Q1 = getPercentile(sortedData, 0.25);
    float Q2 = getPercentile(sortedData, 0.50);  // Median
    float Q3 = getPercentile(sortedData, 0.75);

    // Calculate Mode (most frequent number)
    std::map<int, int> frequency;
    for (int num : sortedData)
        frequency[num]++;

    int mode = sortedData[0];
    int maxFreq = 0;
    for (const auto& [num, freq] : frequency) {
        if (freq > maxFreq) {
            maxFreq = freq;
            mode = num;
        }
    }

    // Print Results
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Q1 (25th Percentile)  : " << Q1 << '\n';
    std::cout << "Median (Q2, 50th)     : " << Q2 << '\n';
    std::cout << "Q3 (75th Percentile)  : " << Q3 << '\n';
    std::cout << "Mode                  : " << mode << " (Frequency: " << maxFreq << ")\n";
    std::cout << "Range                 : " << range << '\n';
}






int main() {



    Biome::registerBiomes();
    auto console = lce::CONSOLE::WIIU;
    auto version = LCEVERSION::ELYTRA;
    // -6651998285536156346
    auto seed = 12345;
    auto worldsize = lce::WORLDSIZE::CLASSIC;
    auto biomescale = lce::BIOMESCALE::SMALL;;


    Generator g(console, version, seed, worldsize, biomescale);



    static constexpr u32 VILLAGES_TO_GEN = 10000;


    int pieceArray[VILLAGES_TO_GEN] = {};
    int pieceCount = 0;

    RNG rng = RNG(8147); // 8147

    Timer timer;

    gen::Village village(&g);

    for (int index = 0; index < VILLAGES_TO_GEN; index++) {
        // rng = RNG(8147);
        g.applyWorldSeed(rng.nextLongI());
        int chunkX = 0;
        int chunkZ = 0;
        // auto village_locations = Placement::Village<false>::getAllPositions(&g);

        village.reset();
        village.generate(chunkX, chunkZ);

        pieceCount += village.getPieceCount();
        pieceArray[index] = village.getPieceCount();
    }


    c_float time = timer.getSeconds();
    c_float timePerVillage = time / VILLAGES_TO_GEN;
    c_float piecesPerVillage = static_cast<float>(pieceCount) / VILLAGES_TO_GEN;
    c_float villagesPerSecond = 1 / (timePerVillage);

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\nBenchmark Statistics: \n";
    std::cout << "Villages  Genned    : " << VILLAGES_TO_GEN << "\n";
    std::cout << "Total Time Taken    : " << time << "sec\n";
    std::cout << "Villages Per Sec    : " << villagesPerSecond << "\n";
    std::cout << "Average   Pieces    : " << piecesPerVillage << "\n";

    std::cout << "\n";
    std::cout << "Piece Count Statistics: \n";
    std::vector<int> data(pieceArray, pieceArray + VILLAGES_TO_GEN);
    calculateStatistics(data);



    return 0;
}









