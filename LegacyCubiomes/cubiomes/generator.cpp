#include "generator.hpp"

#include "LegacyCubiomes/utils/range.hpp"
#include "LegacyCubiomes/utils/rng.hpp"
#include "biomeID.hpp"
#include "noise.hpp"


Generator::Generator(const CONSOLE console, const LCEVERSION version, const WORLDSIZE size, const BIOMESCALE scale)
    : worldSeed(0), version(version), console(console), biomeScale(scale), worldSize(size),
      worldCoordinateBounds(getChunkWorldBounds(size) << 4) {
    setupLayerStack(&this->layerStack, version, scale);
    setLayerSeed(this->layerStack.entry_1, 0);
}


Generator::Generator(const CONSOLE console, const LCEVERSION version, const int64_t seed, const WORLDSIZE size,
                     const BIOMESCALE scale)
    : worldSeed(seed), version(version), console(console), biomeScale(scale), worldSize(size),
      worldCoordinateBounds(getChunkWorldBounds(size) << 4) {
    setupLayerStack(&this->layerStack, version, scale);
    setLayerSeed(this->layerStack.entry_1, seed);
}


void Generator::applyWorldSeed(const int64_t seed) {
    this->worldSeed = seed;
    setLayerSeed(this->layerStack.entry_1, seed);
}


void Generator::changeLCEVersion(const LCEVERSION versionIn) {
    // avoid setting up again when it's the same
    if (this->version == versionIn) return;

    this->version = versionIn;
    setupLayerStack(&this->layerStack, versionIn, this->biomeScale);
    //reapply the layers' seed
    setLayerSeed(this->layerStack.entry_1, this->worldSeed);
}


void Generator::changeBiomeSize(const BIOMESCALE size) {
    // avoid setting up again when it's the same
    if (this->biomeScale == size) return;

    this->biomeScale = size;
    setupLayerStack(&this->layerStack, this->version, size);
    //reapply the layers' seed
    setLayerSeed(this->layerStack.entry_1, this->worldSeed);
}


void Generator::changeWorldSize(const WORLDSIZE size) {
    // avoid recalculating when it's the same
    if (this->worldSize == size) return;

    this->worldSize = size;
    this->worldCoordinateBounds = getChunkWorldBounds(size) << 4;
}


size_t Generator::getMinCacheSize(const int scale, const int sx, const int sz) const {
    // recursively check the layer stack for the max buffer
    const Layer* layerForScale = getLayerForScale(scale);
    if (!layerForScale) {
        printf("getMinCacheSize(): failed to determine scaled layerForScale\n");
        exit(1);
    }
    return getMinLayerCacheSize(layerForScale, sx, sz);
}

///TODO: make it int8_t array as biome ids don't go higher than 255; will need to refactor all uses of biomes
int* Generator::allocCache(const Range& range) const {
    const size_t len = getMinCacheSize(range.scale, range.sx, range.sz);
    return (int*) calloc(len, sizeof(int));
}


int Generator::genBiomes(int* cache, const Range& range) const {
    const Layer* layerForScale = getLayerForScale(range.scale);
    if (!layerForScale) return -1;
    const int err = genArea(layerForScale, cache, range.x, range.z, range.sx, range.sz);
    if (err) return err;

    return 0;
}


int Generator::getBiomeAt(const int scale, const int x, const int z) const {
    const Range r = {scale, x, z, 1, 1};
    int* ids = allocCache(r);
    int id = genBiomes(ids, r);

    if (id == 0) id = ids[0];
    else
        id = BiomeID::none;
    free(ids);
    return id;
}


int* Generator::getBiomeRange(const int scale, const int x, const int z, const int w, const int h) const {
    const Range r = {scale, x, z, w, h};
    int* ids = allocCache(r);
    genBiomes(ids, r);
    return ids;
}


std::pair<int, int*> Generator::generateAllBiomes() const {
    // Small World Size
    int size = getChunkWorldBounds(worldSize) << 2;
    const Range r = {4, -size, -size, size * 2, size * 2};
    int* ids = allocCache(r);
    genBiomes(ids, r);
    size = getWorldCoordinateBounds() >> 1;
    return {size, ids};
}


Layer* Generator::getLayerForScale(const int scale) const {
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

bool Generator::areBiomesViable(const int x, const int z, const int rad, const uint64_t validBiomes,
                                const uint64_t mutatedValidBiomes) const {
    if (x - rad < -this->worldCoordinateBounds || x + rad >= this->worldCoordinateBounds ||
        z - rad < -this->worldCoordinateBounds || z + rad >= this->worldCoordinateBounds) {
        return false;
    }

    bool viable;
    int i;
    int id;
    int* ids = nullptr;
    int x1 = (x - rad) >> 2, x2 = (x + rad) >> 2, sx = x2 - x1 + 1;
    int z1 = (z - rad) >> 2, z2 = (z + rad) >> 2, sz = z2 - z1 + 1;

    if (rad > 5) {
        // check corners
        const Pos2D corners[4] = {{x1, z1}, {x2, z2}, {x1, z2}, {x2, z1}};
        for (i = 0; i < 4; i++) {
            id = getBiomeAt(4, corners[i].x, corners[i].z);
            if (id < 0 || !id_matches(id, validBiomes, mutatedValidBiomes)) goto L_no;
        }
    }

    viable = true;
    {
        const Range r = {4, x1, z1, sx, sz};
        ids = this->allocCache(r);

        if ((viable = !this->genBiomes(ids, r))) {
            for (i = 0; i < sx * sz; i++) {
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


Pos2D Generator::locateBiome(int x, int z, int radius, uint64_t validBiomes, RNG& rng, int* passes) const {
    Pos2D out = {x, z};
    int i, found;
    found = 0;

    const int x1 = (x - radius) >> 2;
    const int z1 = (z - radius) >> 2;
    const int x2 = (x + radius) >> 2;
    const int z2 = (z + radius) >> 2;
    const int width = x2 - x1 + 1;
    const int height = z2 - z1 + 1;

    const Range r = {4, x1, z1, width, height};
    int* ids = allocCache(r);
    genBiomes(ids, r);

    for (i = 0; i < width * height; i++) {
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


int Generator::mapApproxHeight(float* y, int* ids, const SurfaceNoise* sn, const int x, const int z, const int w,
                               const int h) const {

    constexpr float biome_kernel[25] = {
            // with 10 / (sqrt(i**2 + j**2) + 0.2)
            3.302044127, 4.104975761, 4.545454545, 4.104975761, 3.302044127, 4.104975761, 6.194967155,
            8.333333333, 6.194967155, 4.104975761, 4.545454545, 8.333333333, 50.00000000, 8.333333333,
            4.545454545, 4.104975761, 6.194967155, 8.333333333, 6.194967155, 4.104975761, 3.302044127,
            4.104975761, 4.545454545, 4.104975761, 3.302044127,
    };

    auto* depth = (double*) malloc(sizeof(double) * 2 * w * h);
    double* scale = depth + w * h;
    int64_t i, j;
    int ii, jj;

    const Range r = {4, x - 2, z - 2, w + 5, h + 5};

    int* cache = allocCache(r);
    genBiomes(cache, r);

    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            double d0, s0;
            double wt = 0, ws = 0, wd = 0;
            const int id0 = cache[(j + 2) * r.sx + (i + 2)];
            getBiomeDepthAndScale(id0, &d0, &s0, 0);

            for (jj = 0; jj < 5; jj++) {
                for (ii = 0; ii < 5; ii++) {
                    double d, s;
                    int id = cache[(j + jj) * r.sx + (i + ii)];
                    getBiomeDepthAndScale(id, &d, &s, 0);
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
            int px = x + i, pz = z + j;
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
                int k;
                for (k = 0; k < 2; k++) {
                    int py = ytest + k;
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
                ytest += (int) dy;
                if (ytest <= ymin) ytest = ymin + 1;
                if (ytest >= ymax) ytest = ymax - 1;
            } while (ymax - ymin > 1);

            y[j * w + i] = 8 * (vmin / (double) (vmin - vmax) + ymin);
        }
    }
    free(depth);
    return 0;
}


const uint64_t Generator::spawn_biomes = (1ULL << forest) | (1ULL << plains) | (1ULL << taiga) | (1ULL << taiga_hills) |
                                         (1ULL << wooded_hills) | (1ULL << jungle) | (1ULL << jungle_hills);


Pos2D Generator::estimateSpawn(RNG& rng) const {
    int found;

    rng.setSeed(getWorldSeed());
    Pos2D spawn = locateBiome(0, 0, 256, Generator::spawn_biomes, rng, &found);
    if (!found) spawn.x = spawn.z = 8;

    return spawn;
}


Pos2D Generator::getSpawnBlock() const {
    RNG rng;
    Pos2D spawn = estimateSpawn(rng);

    SurfaceNoise sn;
    initSurfaceNoise(&sn, DIMENSION::OVERWORLD, getWorldSeed());

    float y;
    int id = -1, grass = 0;
    for (int i = 0; i < 1000; i++) {
        mapApproxHeight(&y, &id, &sn, spawn.x >> 2, spawn.z >> 2, 1, 1);
        getBiomeDepthAndScale(id, nullptr, nullptr, &grass);

        if (grass > 0 && y >= (float) grass) break;

        spawn.x += rng.nextInt(64) - rng.nextInt(64);
        spawn.z += rng.nextInt(64) - rng.nextInt(64);

        if (spawn.x > worldCoordinateBounds || spawn.x < -worldCoordinateBounds) spawn.x = 0;

        if (spawn.z > worldCoordinateBounds || spawn.z < -worldCoordinateBounds) spawn.z = 0;
    }

    return spawn;
}
