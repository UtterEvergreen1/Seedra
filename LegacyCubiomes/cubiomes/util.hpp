#pragma once
#include "layers.hpp"

/// get the resource id name for a biome (for versions 1.13+)
const char *biome2str(int id);

const char* getStructureName(StructureType type);

/// initialize a biome colormap with some defaults
MU void initBiomeColors(unsigned char biomeColors[256][3]);
MU void initBiomeTypeColors(unsigned char biomeColors[256][3]);

/// Attempts to parse a biome-color mappings from a text buffer.
/// The parser makes one attempt per line and is not very picky regarding a
/// combination of biomeID/name with a color, represented as either a single
/// number or as a triplet in decimal or as hex (preceeded by 0x or #).
/// Returns the number of successfully mapped biome ids
MU int parseBiomeColors(unsigned char biomeColors[256][3], const char *buf);

MU int biomesToImage(unsigned char *pixels,
                  unsigned char biomeColors[256][3], const int *biomes,
                  unsigned int sx, unsigned int sy,
                  unsigned int pixScale, int flip);

