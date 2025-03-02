#include "generator.hpp"

#include "common/range.hpp"
#include "common/rng.hpp"
#include "terrain/biomes/biomeID.hpp"
#include "terrain/noise/noise.hpp"


c_u64 Generator::SPAWN_BIOMES = (1ULL << forest) | (1ULL << plains) | (1ULL << taiga) | (1ULL << taiga_hills) |
                                (1ULL << wooded_hills) | (1ULL << jungle) | (1ULL << jungle_hills);


/**
 * Sets up a biome generator for a given LCE version.
 *
 * @param console target console
 * @param version update version
 * @param size the world size for calculating world bounds
 * @param scale the biome size for generating biomes
 */
Generator::Generator(const lce::CONSOLE console, const LCEVERSION version,
                     const lce::WORLDSIZE size, const lce::BIOMESCALE scale)
    : worldSeed(0), version(version), console(console), biomeScale(scale), worldSize(size),
      worldCoordinateBounds(getChunkWorldBounds(size) << 4) {
    setupLayerStack(&this->layerStack, version, scale);
    setLayerSeed(this->layerStack.entry_1, 0);
}

/**
 * Sets up a biome generator for a given LCE version.
 *
 * @param console target console
 * @param version update version
 * @param scale the biome size for generating biomes
 * @param size the world size for calculating world bounds
 * @param seed the world seed to apply
 */
Generator::Generator(const lce::CONSOLE console, const LCEVERSION version, c_i64 seed, const lce::WORLDSIZE size,
                     const lce::BIOMESCALE scale)
    : worldSeed(seed), version(version), console(console), biomeScale(scale), worldSize(size),
      worldCoordinateBounds(getChunkWorldBounds(size) << 4) {
    setupLayerStack(&this->layerStack, version, scale);
    setLayerSeed(this->layerStack.entry_1, seed);
}


/**
 * Sets up a biome generator for a given LCE version.
 *
 * @param console target console
 * @param version update version
 * @param scale the biome size for generating biomes
 * @param size the world size for calculating world bounds
 * @param seed the world seed to apply
 */
Generator::Generator(const lce::CONSOLE console, const LCEVERSION version, const std::string& seed, const lce::WORLDSIZE size,
                     const lce::BIOMESCALE scale)
    : version(version), console(console), biomeScale(scale), worldSize(size),
      worldCoordinateBounds(getChunkWorldBounds(size) << 4) {
    applyWorldSeed(seed);
    setupLayerStack(&this->layerStack, version, scale);
    setLayerSeed(this->layerStack.entry_1, worldSeed);
}


/**
 * Initializes the generator for a given world seed in the overworld.
 *
 * @param seed world seed to apply
 */
void Generator::applyWorldSeed(c_i64 seed) {
    // avoid setting up again when it's the same
    if (this->worldSeed == seed) return;

    this->worldSeed = seed;
    setLayerSeed(this->layerStack.entry_1, seed);
}


/**
 * Initializes the generator for a given world seed in the overworld.
 *
 * @param seed world seed to apply
 */
void Generator::applyWorldSeed(const std::string& seed) {
    applyWorldSeed(StringHash::hash(seed));
}



/**
 * Change the version of LCE.
 *
 * @param versionIn new LCE version to apply
 */
MU void Generator::changeLCEVersion(const LCEVERSION versionIn) {
    // avoid setting up again when it's the same
    if (this->version == versionIn) return;

    this->version = versionIn;
    setupLayerStack(&this->layerStack, versionIn, this->biomeScale);
    //reapply the layers' seed
    setLayerSeed(this->layerStack.entry_1, this->worldSeed);
}

/**
 * Change the biome size.
 *
 * @param size new biome size to apply
 */
MU void Generator::changeBiomeSize(const lce::BIOMESCALE size) {
    // avoid setting up again when it's the same
    if (this->biomeScale == size) return;

    this->biomeScale = size;
    setupLayerStack(&this->layerStack, this->version, size);
    //reapply the layers' seed
    setLayerSeed(this->layerStack.entry_1, this->worldSeed);
}

/**
 * Change the world size.
 *
 * @param size new world size to apply
 */
MU void Generator::changeWorldSize(const lce::WORLDSIZE size) {
    // avoid recalculating when it's the same
    if (this->worldSize == size) return;

    this->worldSize = size;
    this->worldCoordinateBounds = getChunkWorldBounds(size) << 4;
}

/**
 * Calculates the buffer size (number of ints) required to generate a 2D plane
 * The function allocCache() can be used afterwards to allocate the corresponding int
 * buffer using malloc().
 *
 * @param scale the scale for generating biomes
 * @param sx, sz width and height to generate
 * @return size to malloc()
 */
size_t Generator::getMinCacheSize(c_int scale, c_int sx, c_int sz) const {
    // recursively check the layer stack for the max buffer
    const Layer* layerForScale = getLayerForScale(scale);
    if (!layerForScale) {
        printf("getMinCacheSize(): failed to determine scaled layerForScale\n");
        exit(1);
    }
    return getMinLayerCacheSize(layerForScale, sx, sz);
}

/// TODO: make it i8 array as biome ids don't go higher than 255; will need to refactor all uses of biomes
/**
 * Allocates the biome cache given the range.
 *
 * @param range
 * @return pointer to the biome cache
 */
int* Generator::allocCache(const Range& range) const {
    const size_t len = getMinCacheSize(range.scale, range.sx, range.sz);
    return static_cast<int*>(calloc(len, sizeof(int)));
}

/**
 * Generates the biomes for a 2D plane scaled range given by 'r'.
 * The required length of the cache can be determined with getMinCacheSize().
 *
 * @param[in,out] cache input: allocated cache
 * output: generated biomes in cache,
 * biome ids can be accessed by indexing as: cache[ z * r.sx + x ]
 * where (x,z) is a relative position inside the 2D plane.
 * @param[in] range the range to generate the biomes in
 * @return zero upon success
 */
int Generator::genBiomes(int* cache, const Range& range) const {
    const Layer* layerForScale = getLayerForScale(range.scale);
    if (!layerForScale) return -1;
    c_int err = genArea(layerForScale, cache, range.x, range.z, range.sx, range.sz);
    if (err) return err;

    return 0;
}

/**
 * Gets the biome for a specified scaled position. Note that the scale should
 * be either 1 or 4, for block or biome coordinates respectively.
 *
 * @param scale the scale for generating biomes
 * @param x, z coordinates to generate the biome at
 * @return biome id or -1 if failed
 */
int Generator::getBiomeAt(c_int scale, c_int x, c_int z) const {
    const Range r = {scale, x, z, 1, 1};
    int* ids = allocCache(r);
    int id = genBiomes(ids, r);

    if (id == 0) id = ids[0];
    else
        id = BiomeID::none;
    free(ids);
    return id;
}

/**
 * Generates a biome range (x -> x + w, z -> z + h).
 *
 * @param scale the scale for generating biomes
 * @param x, z top left coordinates to generate
 * @param w, h width and height to generate
 * @return Cache of generated biomes.
 */
int* Generator::getBiomeRange(c_int scale, c_int x, c_int z, c_int w, c_int h) const {
    const Range r = {scale, x, z, w, h};
    int* ids = allocCache(r);
    genBiomes(ids, r);
    return ids;
}

/** Generates all biomes, and returns a std::pair of
 *
 * @return std::pair<N, *ids> where N is the NxN size of array size,
 * and ids is the pointer to that data.
 */
std::pair<int, int*> Generator::generateAllBiomes() const {
    // Small World Size
    int size = getChunkWorldBounds(worldSize) << 2;
    const Range r = {4, -size, -size, size * 2, size * 2};
    int* ids = allocCache(r);
    genBiomes(ids, r);
    size = getWorldCoordinateBounds() >> 1;
    return {size, ids};
}

/**
 * Returns the default layer that corresponds to the given scale.
 *
 * @param scale the supported scales are {1, 4, 16, 64, 256}.
 */
Layer* Generator::getLayerForScale(c_int scale) const {
    switch (scale) {
        case 1:
            return this->layerStack.entry_1;
        case 4:
            return this->layerStack.entry_4;
        case 16:
            return this->layerStack.entry_16;
        case 64:
            return this->layerStack.entry_64;
        case 256:
            return this->layerStack.entry_256;
        default:
            return nullptr;
    }
}


//==============================================================================
// Checking Biomes & Biome Helper Functions
//==============================================================================

/**
 * Checks the surrounding 'rad' blocks from origin (x, z) for all valid biomes.
 *
 * @param x, z center coordinates to check valid biomes at
 * @param rad block radius to check for valid biomes
 * @param validBiomes u64 value of the valid base biomes
 * @param mutatedValidBiomes u64 value of the valid mutated biomes
 * @return true if all the biomes are valid within the radius
 */
bool Generator::areBiomesViable(c_int x, c_int z, c_int rad, c_u64 validBiomes,
                                c_u64 mutatedValidBiomes) const {
    if (x - rad < -this->worldCoordinateBounds || x + rad >= this->worldCoordinateBounds ||
        z - rad < -this->worldCoordinateBounds || z + rad >= this->worldCoordinateBounds) {
        return false;
    }



    int* ids = nullptr;
    int x1 = (x - rad) >> 2, x2 = (x + rad) >> 2;
    const int sx = x2 - x1 + 1;
    int z1 = (z - rad) >> 2, z2 = (z + rad) >> 2;
    const int sz = z2 - z1 + 1;

    bool viable;

    if (rad > 5) {
        // check corners
        const Pos2D corners[4] = {{x1, z1}, {x2, z2}, {x1, z2}, {x2, z1}};
        for (int i = 0; i < 4; i++) {
            const int id = getBiomeAt(4, corners[i].x, corners[i].z);
            if (id < 0 || !id_matches(id, validBiomes, mutatedValidBiomes)) goto L_no;
        }
    }

    viable = true;
    {
        const Range r = {4, x1, z1, sx, sz};
        ids = this->allocCache(r);

        if ((viable = !this->genBiomes(ids, r))) {
            for (int i = 0; i < sx * sz; i++) {
                if (!id_matches(ids[i], validBiomes, mutatedValidBiomes)) goto L_no;
            }
        }
    }
    if (false)
    L_no:
        viable = false;
    if (ids) free(ids);
    return viable;
}

/**
 * Finds a valid biome within 'rad' blocks from origin (x, z) with the rng state 'rng'.
 *
 * @param[in] x center coordinates to check valid biomes at
 * @param[in] z center coordinates to check valid biomes at
 * @param[in] radius block radius to find valid biomes
 * @param[in] validBiomes u64 value of the valid base biomes
 * @param[in] rng pointer to the rng state
 * @param[out] passes returns the total amount of positions picked
 * @return the found position, not found if passes = 0
 */
Pos2D Generator::locateBiome(const int x, const int z, const int radius,
    const u64 validBiomes, RNG& rng, int* passes) const {
    Pos2D out = {x, z};

    int found = 0;

    c_int x1 = (x - radius) >> 2;
    c_int z1 = (z - radius) >> 2;
    c_int x2 = (x + radius) >> 2;
    c_int z2 = (z + radius) >> 2;
    c_int width = x2 - x1 + 1;
    c_int height = z2 - z1 + 1;

    const Range r = {4, x1, z1, width, height};
    int* ids = allocCache(r);
    genBiomes(ids, r);

    for (int i = 0; i < width * height; i++) {
        if (!id_matches(ids[i], validBiomes)) continue;
        if (found == 0 || rng.nextInt(found + 1) == 0) {
            out.x = (x1 + i % width) * 4;
            out.z = (z1 + i / width) * 4;
            ++found;
        }
    }

    free(ids);

    if (passes != nullptr) *passes = found;

    return out;
}

/**
 * Generates the approximate terrain height range (x -> x + w, z -> z + h) into 'y'.
 *
 * @param[in,out] y Pointer to the cached y values
 * @param[out] ids if 'ids' != 0, it will store the biome id
 * @param[in] sn SurfaceNoise instance pointer
 * @param[in] x, z top left coordinates to generate
 * @param[in] w, h width and height to generate
 * @return zero on success
 */
int Generator::mapApproxHeight(float* y, int* ids, const SurfaceNoise* sn,
    c_int x, c_int z, c_int w, c_int h) const {

    // with 10 / (sqrt(i**2 + j**2) + 0.2)
    constexpr float biome_kernel[25] = {
            3.302044127, 4.104975761, 4.545454545, 4.104975761, 3.302044127, 4.104975761, 6.194967155,
            8.333333333, 6.194967155, 4.104975761, 4.545454545, 8.333333333, 50.00000000, 8.333333333,
            4.545454545, 4.104975761, 6.194967155, 8.333333333, 6.194967155, 4.104975761, 3.302044127,
            4.104975761, 4.545454545, 4.104975761, 3.302044127,
    };

    auto* depth = static_cast<double*>(malloc(sizeof(double) * 2 * w * h));
    double* scale = depth + w * h;
    i64 i, j;

    const Range r = {4, x - 2, z - 2, w + 5, h + 5};

    int* cache = allocCache(r);
    genBiomes(cache, r);

    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            double d0, s0;
            double wt = 0, ws = 0, wd = 0;
            c_int id0 = cache[(j + 2) * r.sx + (i + 2)];
            getBiomeDepthAndScale(id0, &d0, &s0, nullptr);

            for (int jj = 0; jj < 5; jj++) {
                for (int ii = 0; ii < 5; ii++) {
                    double d, s;
                    const int id = cache[(j + jj) * r.sx + (i + ii)];
                    getBiomeDepthAndScale(id, &d, &s, nullptr);
                    // TODO: should this stay float, or become double?
                    float weight = biome_kernel[jj * 5 + ii] / (d + 2);
                    if (d > d0) weight *= 0.5;
                    ws += s * weight;
                    wd += d * weight;
                    wt += weight;
                }
            }
            ws /= wt;
            wd /= wt;
            ws = ws * 0.9 + 0.1;
            wd = (wd * 4.0 - 1) / 8;
            ws = 96 / ws;
            wd = wd * 17. / 64;
            depth[j * w + i] = wd;
            scale[j * w + i] = ws;
            if (ids) ids[j * w + i] = id0;
        }
    }
    free(cache);

    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            const int px = x + i, pz = z + j;
            double off = sampleOctaveAmp(this, &sn->octaveDepth, px * 200, 10, pz * 200, 1, 0, 1);
            off *= 65535. / 8000;
            if (off < 0) off = -0.3 * off;
            off = off * 3 - 2;
            if (off > 1) off = 1;
            off *= 17. / 64;
            if (off < 0) off *= 1. / 28;
            else
                off *= 1. / 40;

            double vmin = 0, vmax = 0;
            int ytest = 8, ymin = 0, ymax = 32;
            do {
                double v[2];
                for (int k = 0; k < 2; k++) {
                    const int py = ytest + k;
                    double n0 = sampleSurfaceNoise(this, sn, px, py, pz);
                    double fall = 1 - 2 * py / 32.0 + off - 0.46875;
                    fall = scale[j * w + i] * (fall + depth[j * w + i]);
                    n0 += (fall > 0 ? 4 * fall : fall);
                    v[k] = n0;
                    if (n0 >= 0 && py > ymin) {
                        ymin = py;
                        vmin = n0;
                    }
                    if (n0 < 0 && py < ymax) {
                        ymax = py;
                        vmax = n0;
                    }
                }
                double dy = v[0] / (v[0] - v[1]);
                dy = (dy <= 0 ? floor(dy) : ceil(dy)); // round away from zero
                ytest += static_cast<int>(dy);
                if (ytest <= ymin) ytest = ymin + 1;
                if (ytest >= ymax) ytest = ymax - 1;
            } while (ymax - ymin > 1);

            y[j * w + i] = 8 * (vmin / (vmin - vmax) + ymin);
        }
    }
    free(depth);
    return 0;
}


/**
 * Estimates the spawn by only calling locateBiome.
 *
 * @param rng the current rng state
 * @return found spawn block coordinates, if not found, then (8, 8)
 */
Pos2D Generator::estimateSpawn(RNG& rng) const {
    int found;

    rng.setSeed(getWorldSeed());
    Pos2D spawn = locateBiome(0, 0, 256, Generator::SPAWN_BIOMES, rng, &found);
    if (!found) spawn.x = spawn.z = 8;

    return spawn;
}

/// Finds the spawn block coordinates (not currently correct in wooded_badlands_plateau or mesa_plateau_stone).
MU Pos2D Generator::getSpawnBlock() const {
    RNG rng;
    Pos2D spawn = estimateSpawn(rng);

    SurfaceNoise sn;
    initSurfaceNoise(&sn, lce::DIMENSION::OVERWORLD, getWorldSeed());

    float y;
    int id = -1, grass = 0;
    for (int i = 0; i < 1000; i++) {
        mapApproxHeight(&y, &id, &sn, spawn.x >> 2, spawn.z >> 2, 1, 1);
        getBiomeDepthAndScale(id, nullptr, nullptr, &grass);

        if (grass > 0 && y >= static_cast<float>(grass)) break;

        spawn.x += rng.nextInt(64) - rng.nextInt(64);
        spawn.z += rng.nextInt(64) - rng.nextInt(64);

        if (spawn.x > worldCoordinateBounds || spawn.x < -worldCoordinateBounds) spawn.x = 0;

        if (spawn.z > worldCoordinateBounds || spawn.z < -worldCoordinateBounds) spawn.z = 0;
    }

    return spawn;
}

/**
 * Overload function for that allows for using Pos2D as position in locateBiome.
 *
 * @see Pos2D locateBiome(int x, int z, int radius, c_u64& validBiomes, u64* rng, int* passes) const
 * @param[in] pos center coordinates to check valid biomes at
 * @param[in] radius block radius to find valid biomes
 * @param[in] validBiomes u64 value of the valid base biomes
 * @param[in] rng pointer to the rng state
 * @param[out] passes returns the total amount of positions picked
 * @return the found position, not found if passes = 0
 */
Pos2D Generator::locateBiome(const Pos2D pos, const int radius, const uint64_t validBiomes, RNG& rng,
                             int* passes) const {
    return locateBiome(pos.x, pos.z, radius, validBiomes, rng, passes);
}

/**
 * Overload function for that allows for using Pos2D as position in areBiomesViable.
 *
 * @see bool areBiomesViable(int x, int z, int rad, const char* validBiomes) const
 * @param pos center coordinates to check valid biomes at
 * @param rad block radius to check for valid biomes
 * @param validBiomes u64 value of the valid base biomes
 * @param mutatedValidBiomes u64 value of the valid mutated biomes
 * @return true if all the biomes are valid within the radius
 */
MU bool Generator::areBiomesViable(const Pos2D pos, const int rad, const uint64_t validBiomes,
                                const uint64_t mutatedValidBiomes) const {
    return areBiomesViable(pos.x, pos.z, rad, validBiomes, mutatedValidBiomes);
}

/**
 * Checks the given id against the valid biomes.
 *
 * @param id biome id to check
 * @param validBiomes u64 value of the valid base biomes
 * @param mutatedValidBiomes u64 value of the valid mutated biomes
 * @return true if the biome id exists in the valid biomes
 */
bool Generator::id_matches(const int id, const uint64_t validBiomes, const uint64_t mutatedValidBiomes) {
    return id < 128 ? (validBiomes & (1ULL << id)) != 0 : (mutatedValidBiomes & (1ULL << (id - 128))) != 0;
}

/**
 * Overload function for that allows for using Pos2D as position in genBiomes.
 *
 * @see int getBiomeAt(int scale, int x, int z) const
 * @param scale the scale for generating biomes
 * @param pos coordinates to generate the biome at
 * @return biome id or -1 if failed
 */
int Generator::getBiomeAt(const int scale, const Pos2D pos) const { return getBiomeAt(scale, pos.x, pos.z); }


/**
 * Overload function for that allows for using Pos2D as position in genBiomes.
 *
 * @see int getBiomeAt(int scale, int x, int z) const
 * @param scale the scale for generating biomes
 * @param pos coordinates to generate the biome at
 * @return biome id or -1 if failed
 */
int Generator::getBiomeAt(const int scale, const Pos3D pos) const { return getBiomeAt(scale, pos.x, pos.z); }
