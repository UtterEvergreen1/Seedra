#pragma once

#include "LegacyCubiomes/utils/enums.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class Generator;


struct PerlinNoise {
    uint8_t d[512];
    double a;
    double b;
    double c;
    double amplitude;
    double lacunarity;
};


struct OctaveNoise {
    int octcnt{};
    PerlinNoise* octaves = nullptr;
};


struct SurfaceNoise {
    double xzScale{}, yScale{};
    double xzFactor{}, yFactor{};
    OctaveNoise octaveMin;
    OctaveNoise octaveMax;
    OctaveNoise octaveMain;
    OctaveNoise octaveSurf;
    OctaveNoise octaveDepth;
    PerlinNoise oct[16 + 16 + 8 + 4 + 16]{};
};


/// Helper
double maintainPrecision(double x);

/// Perlin noise
void perlinInit(PerlinNoise* noise, RNG& rng);

double samplePerlin(const Generator* g, const PerlinNoise* noise, double x, double y, double z, double yAmp,
                    double yMin);
double sampleSimplex2D(const PerlinNoise* noise, double x, double y);

/// Perlin Octaves
void octaveInit(OctaveNoise* noise, RNG& rng, PerlinNoise* octaves, int oMin, int len);

MU double sampleOctave(const Generator* g, const OctaveNoise* noise, double x, double y, double z);

double sampleOctaveAmp(const Generator* g, const OctaveNoise* noise, double x, double y, double z, double yAmp,
                       double yMin, int yDefault);

void initSurfaceNoise(SurfaceNoise* sn, DIMENSION dimension, uint64_t worldSeed);
