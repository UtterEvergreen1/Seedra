#include "terrain/Chunk.hpp"
#include "terrain/ChunkPrimer.hpp"
#include "terrain/World.hpp"
#include "terrain/biomes/biome.hpp"

#include <array>

// A 4x4 array of ints:
using SubChunkGrid = std::array<std::array<int, 4>, 4>;

bool isAirBlock(uint16_t blockId) {
    // Suppose '0' means air. You might have a different criterion.
    return (blockId == 0);
}

SubChunkGrid buildSubChunkAirCountForChunk(const ChunkPrimer* chunk) {
    SubChunkGrid result{};
    // Each chunk is 16×256×16. We break that into (16/4=4) by (16/4=4) sub-chunks.
    // Each sub-chunk is 4×256×4.
    for (int sx = 0; sx < 4; sx++) {
        for (int sz = 0; sz < 4; sz++) {
            int airCount = 0;
            int baseX = sx * 4;
            int baseZ = sz * 4;
            for (int dx = 0; dx < 4; dx++) {
                for (int dz = 0; dz < 4; dz++) {
                    for (int y = 0; y < 256; y++) {
                        // YZX indexing: index = y + (z*256) + (x*256*16)
                        int xCoord = baseX + dx;
                        int zCoord = baseZ + dz;
                        int idx = y + (zCoord * 256) + (xCoord * 256 * 16);
                        if (isAirBlock(chunk->blocks[idx])) { airCount++; }
                    }
                }
            }
            result[sx][sz] = airCount;
        }
    }
    return result;
}


#include <algorithm> // std::max
#include <iostream>
#include <vector>

// Build the global sub-chunk array for all chunks in [minChunkX..maxChunkX, minChunkZ..maxChunkZ].
void buildGlobalSubchunkArray(World* world, int minChunkX, int maxChunkX, int minChunkZ, int maxChunkZ,
                              std::vector<std::vector<int>>& subChunkAirGrid) {
    int numChunksX = (maxChunkX - minChunkX + 1);
    int numChunksZ = (maxChunkZ - minChunkZ + 1);
    int globalSizeX = numChunksX * 4;
    int globalSizeZ = numChunksZ * 4;

    // Resize our 2D array [globalSizeX][globalSizeZ]
    subChunkAirGrid.resize(globalSizeX, std::vector<int>(globalSizeZ, 0));

    for (int cx = minChunkX; cx <= maxChunkX; cx++) {
        for (int cz = minChunkZ; cz <= maxChunkZ; cz++) {
            // Provide or get the chunk from the world
            ChunkPrimer* chunk = Chunk::provideNewChunk(*world->getGenerator(), {cx, cz});
            // ChunkPrimer* chunk = world->getChunk({cx, cz});
            if (!chunk) {
                // handle missing chunk
                continue;
            }
            // Build its 4x4 sub-chunk air counts
            SubChunkGrid localGrid = buildSubChunkAirCountForChunk(chunk);

            // Now place that 4x4 data into our big array.
            int baseGX = (cx - minChunkX) * 4; // global sub-chunk X start index
            int baseGZ = (cz - minChunkZ) * 4; // global sub-chunk Z start index

            for (int sx = 0; sx < 4; sx++) {
                for (int sz = 0; sz < 4; sz++) { subChunkAirGrid[baseGX + sx][baseGZ + sz] = localGrid[sx][sz]; }
            }
        }
    }
}


void buildPrefixSum2D(const std::vector<std::vector<int>>& src, std::vector<std::vector<int>>& prefix) {
    int sizeX = (int) src.size();
    if (sizeX == 0) return;
    int sizeZ = (int) src[0].size();

    prefix.resize(sizeX, std::vector<int>(sizeZ, 0));

    for (int x = 0; x < sizeX; x++) {
        for (int z = 0; z < sizeZ; z++) {
            int val = src[x][z];
            int left = (x > 0) ? prefix[x - 1][z] : 0;
            int up = (z > 0) ? prefix[x][z - 1] : 0;
            int diag = (x > 0 && z > 0) ? prefix[x - 1][z - 1] : 0;
            prefix[x][z] = val + left + up - diag;
        }
    }
}

// Now a helper to quickly sum a region in prefix array:
int sumRegion2D(const std::vector<std::vector<int>>& prefix, int x1, int z1, int x2, int z2) {
    // sums inclusive region [x1..x2, z1..z2]
    if (x1 > x2 || z1 > z2) return 0;
    int left = (x1 > 0) ? prefix[x1 - 1][z2] : 0;
    int up = (z1 > 0) ? prefix[x2][z1 - 1] : 0;
    int diag = (x1 > 0 && z1 > 0) ? prefix[x1 - 1][z1 - 1] : 0;
    int total = prefix[x2][z2] - left - up + diag;
    return total;
}


struct BestResult {
    int airCount;
    int startSubX; // top-left sub-chunk in global coords
    int startSubZ;
};

BestResult findMaxAirIn16x256x16(const std::vector<std::vector<int>>& prefix) {
    BestResult best{-1, 0, 0};
    int sizeX = (int) prefix.size();
    if (sizeX == 0) return best;
    int sizeZ = (int) prefix[0].size();

    // We look for 4×4 blocks of sub-chunks
    for (int sx = 0; sx + 3 < sizeX; sx++) {
        for (int sz = 0; sz + 3 < sizeZ; sz++) {
            int airSum = sumRegion2D(prefix, sx, sz, sx + 3, sz + 3);
            if (airSum > best.airCount) {
                best.airCount = airSum;
                best.startSubX = sx;
                best.startSubZ = sz;
            }
        }
    }
    return best;
}


#include <iostream>
#include <vector>

void findBest16x256x16Region(World* world, int minChunkX, int maxChunkX, int minChunkZ, int maxChunkZ) {
    // 1) Build the sub-chunk air grid for all chunks in the region
    std::vector<std::vector<int>> subChunkAirGrid;
    buildGlobalSubchunkArray(world, minChunkX, maxChunkX, minChunkZ, maxChunkZ, subChunkAirGrid);

    // 2) Build the 2D prefix sum over subChunkAirGrid
    std::vector<std::vector<int>> prefix;
    buildPrefixSum2D(subChunkAirGrid, prefix);

    // 3) Find the best 4x4 sub-chunk region
    BestResult best = findMaxAirIn16x256x16(prefix);
    if (best.airCount < 0) {
        std::cout << "No valid region found!\n";
        return;
    }

    std::cout << "Best air count = " << best.airCount << " in sub-chunk coords (" << best.startSubX << ", "
              << best.startSubZ << ")\n";

    // 4) Convert sub-chunk coords to chunk & block coords
    int chunkOffsetX = best.startSubX / 4;
    int localSubX = best.startSubX % 4;
    int chunkX = minChunkX + chunkOffsetX;
    int blockX = chunkX * 16 + (localSubX * 4);

    int chunkOffsetZ = best.startSubZ / 4;
    int localSubZ = best.startSubZ % 4;
    int chunkZ = minChunkZ + chunkOffsetZ;
    int blockZ = chunkZ * 16 + (localSubZ * 4);

    std::cout << "Corresponding block start = (" << blockX << ", " << blockZ << ")\n";

    // The 16×16 region is [blockX..blockX+15] × [blockZ..blockZ+15].
}


static constexpr i32 WIDTH = 6;
static constexpr auto CONSOLE = lce::CONSOLE::WIIU;
static constexpr auto VERSION = LCEVERSION::ELYTRA;
static constexpr auto WORLDSIZE = lce::WORLDSIZE::CLASSIC;
static constexpr auto BIOMESCALE = lce::BIOMESCALE::SMALL;


int main() {
    Biome::registerBiomes();


    Generator g(CONSOLE, VERSION, WORLDSIZE, BIOMESCALE);
    World world = World(&g);


    for (int64_t seed = 0; seed < 100; seed++) {
        world.deleteWorld();
        g.applyWorldSeed(-6651998285536156346 + seed);

        RNG rng;
        Pos2D spawn = g.estimateSpawn(rng);

        int X_OFF = spawn.x / 16;
        int Z_OFF = spawn.z / 16;
        findBest16x256x16Region(
                &world, -WIDTH + X_OFF, WIDTH + X_OFF, -WIDTH + Z_OFF, WIDTH + Z_OFF);
    }

    return 0;
}