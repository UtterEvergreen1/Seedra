#pragma once

#include "rng.hpp"

#include "LegacyCubiomes/utils/enums.hpp"

struct PerlinNoise
{
    uint8_t d[512];
    double a;
    double b;
    double c;
    double amplitude;
    double lacunarity;
};

struct OctaveNoise
{
    int octcnt{};
    PerlinNoise *octaves;
};

struct SurfaceNoise
{
    double xzScale, yScale;
    double xzFactor, yFactor;
    OctaveNoise octaveMin;
    OctaveNoise octaveMax;
    OctaveNoise octaveMain;
    OctaveNoise octaveSurf;
    OctaveNoise octaveDepth;
    PerlinNoise oct[16+16+8+4+16];
};

struct DoublePerlinNoise
{
    double amplitude;
    OctaveNoise octA;
    OctaveNoise octB;
};

/// Helper
double maintainPrecision(double x);

/// Perlin noise
void perlinInit(PerlinNoise *noise, uint64_t *seed);

double samplePerlin(const PerlinNoise *noise, double x, double y, double z,
        double yamp, double ymin);
double sampleSimplex2D(const PerlinNoise *noise, double x, double y);

/// Perlin Octaves
void octaveInit(OctaveNoise *noise, uint64_t *seed, PerlinNoise *octaves,
        int omin, int len);

double sampleOctave(const OctaveNoise *noise, double x, double y, double z);

/// Double Perlin
void doublePerlinInit(DoublePerlinNoise *noise, uint64_t *seed,
        PerlinNoise *octavesA, PerlinNoise *octavesB, int omin, int len);

double sampleDoublePerlin(const DoublePerlinNoise *noise,
        double x, double y, double z);

double sampleOctaveAmp(const OctaveNoise *noise, double x, double y, double z,
                       double yamp, double ymin, int ydefault);

void initSurfaceNoise(SurfaceNoise *sn, DIMENSIONS dimension, uint64_t worldSeed);

