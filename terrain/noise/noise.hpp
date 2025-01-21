#pragma once

#include "lce/enums.hpp"
#include "common/rng.hpp"

class Generator;


/// Helper

double maintainPrecision(double x);


/// Perlin noise
struct PerlinNoise {
    u8 d[512];
    double a;
    double b;
    double c;
    double amplitude;
    double lacunarity;
};

void perlinInit(PerlinNoise* noise, RNG& rng);

double samplePerlin(const Generator* g, const PerlinNoise* noise, double x, double y, double z, double yAmp,
                    double yMin);
double sampleSimplex2D(const PerlinNoise* noise, double x, double y);


/// Octave noise
struct OctaveNoise {
    int octcnt{};
    PerlinNoise* octaves = nullptr;
};

void octaveInit(OctaveNoise* noise, RNG& rng, PerlinNoise* octaves, int oMin, int len);

MU double sampleOctave(const Generator* g, const OctaveNoise* noise, double x, double y, double z);

double sampleOctaveAmp(const Generator* g, const OctaveNoise* noise, double x, double y, double z, double yAmp,
                       double yMin, int yDefault);


/// Perlin Octaves
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

void initSurfaceNoise(SurfaceNoise* sn, lce::DIMENSION dimension, u64 worldSeed);
