#pragma once

#include "../utils/pos2D.hpp"
#include "LegacyCubiomes/utils/enums.hpp"
#include "layers.hpp"
#include "noise.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <unordered_set>
#include <iostream>

class Generator {

public:
    int mc;
    int dim;
    int64_t seed;
    WORLDSIZE worldSize;
    BIOMESCALE biomeSize;
    LCEVERSION lceVersion;

    int worldCoordinateBounds;

    union { // Nested class for the union structure
        struct { // Nested structure for MC 1.0 - 1.17
            LayerStack ls;
            Layer xlayer[5]; // buffer for custom entry layers @{1,4,16,64,256}
            Layer* entry;
        };
    };
    //EndNoise en{}; // MC 1.9


    ///=============================================================================
    /// Biome Generation
    ///=============================================================================

    /**
     * Sets up a biome generator for a given MC version. The 'flags' can be used to
     * control LARGE_BIOMES or to FORCE_OCEAN_VARIANTS to enable ocean variants at
     * scales higher than normal.
     */
    Generator(LCEVERSION lceVersion, WORLDSIZE worldSize, BIOMESCALE biomeSize)
        : mc(MC_1_13), dim(1000), seed(0), worldSize(worldSize), biomeSize(biomeSize), lceVersion(lceVersion)
    {
        setupLayerStack(&this->ls, mc, lceVersion, biomeSize);
        worldCoordinateBounds = getChunkWorldBounds(worldSize) << 4;
        this->entry = nullptr;
    }

    Generator(LCEVERSION lceVersion, BIOMESCALE biomeSize)
        : mc(MC_1_13), dim(1000), seed(0), worldSize(WORLDSIZE::CLASSIC), biomeSize(biomeSize), lceVersion(lceVersion)
    {
        setupLayerStack(&this->ls, mc, lceVersion, biomeSize);
        worldCoordinateBounds = getChunkWorldBounds(worldSize) << 4;
        this->entry = nullptr;
    }

    /**
     * Initializes the generator for a given dimension and seed.
     * dim=0:   Overworld
     * dim=-1:  Nether
     * dim=+1:  End
     */
    void applySeed(int theDim, uint64_t theSeed)
    {
        this->dim = theDim;
        this->seed = theSeed;

        if (dim == 0)
            setLayerSeed(this->entry ? this->entry : this->ls.entry_1, seed);
    }

    void applySeed(uint64_t theSeed)
    {
        this->dim = 0;
        this->seed = theSeed;
        setLayerSeed(this->entry ? this->entry : this->ls.entry_1, seed);
    }
    /**
     * Calculates the buffer size (number of ints) required to generate a cuboidal
     * volume of size (sx, sy, sz). If 'sy' is zero the buffer is calculated for a
     * 2D plane (which is equivalent to sy=1 here).
     * The function allocCache() can be used to allocate the corresponding int
     * buffer using malloc().
     */
    [[nodiscard]] size_t getMinCacheSize(int scale, int sx, int sz) const
    {
        size_t len = (size_t)sx * sz;
        // recursively check the layer stack for the max buffer
        const Layer* entry = getLayerForScale(scale);
        if (!entry) {
            printf("getMinCacheSize(): failed to determine scaled entry\n");
            exit(1);
        }
        size_t len2d = getMinLayerCacheSize(entry, sx, sz);
        len += len2d - sx * sz;

        return len;
    }

    /**
     * ...
     * @param r
     * @return
     */
    [[nodiscard]] int* allocCache(const Range& r) const
    {
        size_t len = getMinCacheSize(r.scale, r.sx, r.sz);
        return (int*)calloc(len, sizeof(int));
    }


    /**
     * This function is meant for speed, where you give it your own temp cache.
     * It presumes the scale is 1.
     * It presumes the dimension is the overworld.
     * @param temp_cache
     * @param blockX
     * @param blockZ
     * @return
     */
    int getBiomeAt(int* temp_cache, int scale, int blockX, int blockZ) const
    {

        const Layer* entry = getLayerForScale(scale);
        if (!entry) return -1;

        int err = genArea(entry, temp_cache, blockX, blockZ, 1, 1);
        if (err) return -1;

        return temp_cache[0];

    }



    /**
     * Generates the biomes for a cuboidal scaled range given by 'r'.
     * (See description of Range for more detail.)
     *
     * The output is generated inside the cache. Upon success the biome ids can be
     * accessed by indexing as:
     *  cache[ z * r.sx + x ]
     * where (x,y,z) is an relative position inside the range cuboid.
     *
     * The required length of the cache can be determined with getMinCacheSize().
     *
     * The return value is zero upon success.
     */
    int genBiomes(int* cache, const Range& r)
    {
        int err = 1;
        int i;

        if (this->dim == 0)
        {
            const Layer* entry = getLayerForScale(r.scale);
            if (!entry) return -1;
            err = genArea(entry, cache, r.x, r.z, r.sx, r.sz);
            if (err) return err;

            for (i = 0; i < r.sx * r.sz; i++)
                cache[r.sx * r.sz + i] = cache[i];
            /*
            for (k = 1; k < r.sy; k++)
            {   // overworld has no vertical noise: expanding 2D into 3D
                for (i = 0; i < r.sx*r.sz; i++)
                    cache[k*r.sx*r.sz + i] = cache[i];
            }
            */
            return 0;
        }

        return err;
    }


    /**
     * Gets the biome for a specified scaled position. Note that the scale should
     * be either 1 or 4, for block or biome coordinates respectively.
     * Returns none (-1) upon failure.
     */
    int getBiomeAt(int scale, int x, int z)
    {
        Range r = { scale, x, z, 1, 1 };
        int* ids = allocCache(r);
        int id = genBiomes(ids, r);
        if (id == 0)
            id = ids[0];
        else
            id = none;
        free(ids);
        return id;
    }

    /**
     * Overload that allows for using position
     */
    inline int getBiomeAt(int scale, Pos2D pos)
    {
        return getBiomeAt(scale, pos.x, pos.z);
    }

    /**
     * attempts to generate a biome range, and returns a pointer to the cache.
     */
    [[nodiscard]] int* getBiomeRange(int scale, int x, int z, int sizeX, int sizeZ) {
        Range r = { scale, x, z, sizeX, sizeZ };
        int* ids = allocCache(r);
        genBiomes(ids, r);
        return ids;
    }

    /**
     * attempts to generate all biomes, and returns a pointer to the cache.
     */
    [[nodiscard]] int* generateAllBiomes() {
        Range r = { 1, -432, -432, 864, 864 };
        int* ids = allocCache(r);
        genBiomes(ids, r);
        return ids;
    }

    /**
     * Returns the default layer that corresponds to the given scale.
     * Supported scales are {0, 1, 4, 16, 64, 256}. A scale of zero indicates the
     * custom entry layer 'g->entry'.
     * (Overworld, MC <= 1.17)
     */
    [[nodiscard]] const Layer* getLayerForScale(int scale) const
    {
        switch (scale)
        {
        case 0:   return this->entry;
        case 1:   return this->ls.entry_1;
        case 4:   return this->ls.entry_4;
        case 16:  return this->ls.entry_16;
        case 64:  return this->ls.entry_64;
        case 256: return this->ls.entry_256;
        default:
            return nullptr;
        }
    }

    //==============================================================================
    // Checking Biomes & Biome Helper Functions
    //==============================================================================

    Pos2D locateBiome(int x, int z, int radius, const char* validBiomes, uint64_t* rng, int* passes) {
        Pos2D out = { x, z };
        int i, found;
        found = 0;

        int x1 = (x - radius) >> 2;
        int z1 = (z - radius) >> 2;
        int x2 = (x + radius) >> 2;
        int z2 = (z + radius) >> 2;
        int width = x2 - x1 + 1;
        int height = z2 - z1 + 1;

        Range r = { 4, x1, z1, width, height };
        int* ids = this->allocCache(r);
        this->genBiomes(ids, r);

        for (i = 0; i < width * height; i++)
        {
            if (!validBiomes[ids[i]]) continue;
            if (found == 0 || nextInt(rng, found + 1) == 0)
            {
                //int bi = ids[i];
                out.x = (x1 + i % width) << 2;
                out.z = (z1 + i / width) << 2;
                ++found;
            }
        }

        free(ids);

        if (passes != nullptr)
        {
            *passes = found;
        }

        return out;
    }


    inline int valid_1x1(int x, int z, const Range& r, int* buf, const char* valid)
    {
        int* p = buf + (x - r.x) + (z - r.z) * r.sx + (r.sx * r.sz);
        if (*p)
            return 1;
        *p = -1;
        int id = this->getBiomeAt(4, x, z);
        return valid[id];
    }

    int areBiomesViable(int x, int z, int rad, const char* validBiomes, int approx) {
        if (x - rad < -worldCoordinateBounds || x + rad > worldCoordinateBounds ||
            z - rad < -worldCoordinateBounds || z + rad > worldCoordinateBounds) {
            return 0;
        }

        int x1 = (x - rad) >> 2, x2 = (x + rad) >> 2, sx = x2 - x1 + 1;
        int z1 = (z - rad) >> 2, z2 = (z + rad) >> 2, sz = z2 - z1 + 1;

        Range r = { 4, x1, z1, sx, sz };
        int* ids = this->allocCache(r);
        int i;
        int viable = 1;
        const char* v = validBiomes;

        // check corners
        if (rad >= 10) {
            if (!valid_1x1(x1, z1, r, ids, v)) goto L_no;
            if (!valid_1x1(x2, z2, r, ids, v)) goto L_no;
            if (!valid_1x1(x1, z2, r, ids, v)) goto L_no;
            if (!valid_1x1(x2, z1, r, ids, v)) goto L_no;
        }
        if (approx >= 1) goto L_yes;

        if ((viable = !this->genBiomes(ids, r)))
        {
            for (i = 0; i < sx * sz; i++)
            {
                if (!v[ids[i]])
                    goto L_no;
            }
        }
        if (false) L_yes: viable = 1;
        if (false) L_no:  viable = 0;
        free(ids);
        return viable;
    }

    inline int areBiomesViable(Pos2D pos, int rad, const char* validBiomes, int approx) {
        return areBiomesViable(pos.x, pos.z, rad, validBiomes, approx);
    }

    static int id_matches(int id, uint64_t b, uint64_t m)
    {
        return id < 128 ? !!(b & (1ULL << id)) : !!(m & (1ULL << (id - 128)));
    }

    Pos2D locateBiome(int x, int z, int radius,
        uint64_t validB, uint64_t validM, uint64_t* rng, int* passes)
    {
        Pos2D out = { x, z };
        int i, found;
        found = 0;

        int x1 = (x - radius) >> 2;
        int z1 = (z - radius) >> 2;
        int x2 = (x + radius) >> 2;
        int z2 = (z + radius) >> 2;
        int width = x2 - x1 + 1;
        int height = z2 - z1 + 1;

        Range r = { 4, x1, z1, width, height };
        int* ids = allocCache(r);
        genBiomes(ids, r);


        for (i = 0; i < width * height; i++)
        {
            if (!id_matches(ids[i], validB, validM))
                continue;
            if (found == 0 || nextInt(rng, found + 1) == 0)
            {
                out.x = (x1 + i % width) * 4;
                out.z = (z1 + i / width) * 4;
                ++found;
            }
        }

        free(ids);

        if (passes != nullptr)
        {
            *passes = found;
        }

        return out;
    }



    int mapApproxHeight(float* y, int* ids, const SurfaceNoise* sn,
        int x, int z, int w, int h)
    {
        if (dim != DIMENSIONS::OVERWORLD)
            return 1;

        const float biome_kernel[25] = { // with 10 / (sqrt(i**2 + j**2) + 0.2)
                3.302044127, 4.104975761, 4.545454545, 4.104975761, 3.302044127,
                4.104975761, 6.194967155, 8.333333333, 6.194967155, 4.104975761,
                4.545454545, 8.333333333, 50.00000000, 8.333333333, 4.545454545,
                4.104975761, 6.194967155, 8.333333333, 6.194967155, 4.104975761,
                3.302044127, 4.104975761, 4.545454545, 4.104975761, 3.302044127,
        };

        double* depth = (double*)malloc(sizeof(double) * 2 * w * h);
        double* scale = depth + w * h;
        int64_t i, j;
        int ii, jj;

        Range r = { 4, x - 2, z - 2, w + 5, h + 5 };
        //Range r = {4, x-2, z-2, w+10, h+10};
        int* cache = allocCache(r);
        genBiomes(cache, r);

        for (j = 0; j < h; j++)
        {
            for (i = 0; i < w; i++)
            {
                double d0, s0;
                double wt = 0, ws = 0, wd = 0;
                int id0 = cache[(j + 2) * r.sx + (i + 2)];
                getBiomeDepthAndScale(id0, &d0, &s0, 0);

                for (jj = 0; jj < 5; jj++)
                {
                    for (ii = 0; ii < 5; ii++)
                    {
                        double d, s;
                        int id = cache[(j + jj) * r.sx + (i + ii)];
                        getBiomeDepthAndScale(id, &d, &s, 0);
                        float weight = biome_kernel[jj * 5 + ii] / (d + 2);
                        if (d > d0)
                            weight *= 0.5;
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
                if (ids)
                    ids[j * w + i] = id0;
            }
        }
        free(cache);

        for (j = 0; j < h; j++)
        {
            for (i = 0; i < w; i++)
            {
                int px = x + i, pz = z + j;
                double off = sampleOctaveAmp(&sn->octaveDepth, px * 200, 10, pz * 200, 1, 0, 1);
                off *= 65535. / 8000;
                if (off < 0) off = -0.3 * off;
                off = off * 3 - 2;
                if (off > 1) off = 1;
                off *= 17. / 64;
                if (off < 0) off *= 1. / 28;
                else off *= 1. / 40;

                double vmin = 0, vmax = 0;
                int ytest = 8, ymin = 0, ymax = 32;
                do
                {
                    double v[2];
                    int k;
                    for (k = 0; k < 2; k++)
                    {
                        int py = ytest + k;
                        double n0 = sampleSurfaceNoise(sn, px, py, pz);
                        double fall = 1 - 2 * py / 32.0 + off - 0.46875;
                        fall = scale[j * w + i] * (fall + depth[j * w + i]);
                        n0 += (fall > 0 ? 4 * fall : fall);
                        v[k] = n0;
                        if (n0 >= 0 && py > ymin)
                        {
                            ymin = py;
                            vmin = n0;
                        }
                        if (n0 < 0 && py < ymax)
                        {
                            ymax = py;
                            vmax = n0;
                        }
                    }
                    double dy = v[0] / (v[0] - v[1]);
                    dy = (dy <= 0 ? floor(dy) : ceil(dy)); // round away from zero
                    ytest += (int)dy;
                    if (ytest <= ymin) ytest = ymin + 1;
                    if (ytest >= ymax) ytest = ymax - 1;
                } while (ymax - ymin > 1);

                y[j * w + i] = 8 * (vmin / (double)(vmin - vmax) + ymin);
            }
        }
        free(depth);
        return 0;
    }

    static const uint64_t g_spawn_biomes_17 =
        (1ULL << forest) |
        (1ULL << plains) |
        (1ULL << taiga) |
        (1ULL << taiga_hills) |
        (1ULL << wooded_hills) |
        (1ULL << jungle) |
        (1ULL << jungle_hills);

    Pos2D estimateSpawn(uint64_t* rng)
    {
        Pos2D spawn;
        int found;

        setSeed(rng, seed);
        spawn = locateBiome(0, 0, 256, g_spawn_biomes_17, 0, rng, &found);
        if (!found)
            spawn.x = spawn.z = 8;

        return spawn;
    }

    Pos2D getSpawnBlock() {
        uint64_t rng;
        Pos2D spawn = estimateSpawn(&rng);

        SurfaceNoise sn;
        initSurfaceNoise(&sn, DIMENSIONS::OVERWORLD, seed);

        float y;
        int id = -1, grass = 0;
        for (int i = 0; i < 1000; i++)
        {
            mapApproxHeight(&y, &id, &sn, spawn.x >> 2, spawn.z >> 2, 1, 1);
            getBiomeDepthAndScale(id, 0, 0, &grass);

            if (grass > 0 && y >= grass)
                break;
            spawn.x += nextInt(&rng, 64) - nextInt(&rng, 64);
            spawn.z += nextInt(&rng, 64) - nextInt(&rng, 64);

            if (spawn.x > worldCoordinateBounds || spawn.x < -worldCoordinateBounds) {
                spawn.x = 0;
            }

            if (spawn.z > worldCoordinateBounds || spawn.z < -worldCoordinateBounds) {
                spawn.z = 0;
            }
        };

        return spawn;
    }



};

