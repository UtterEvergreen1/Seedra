#pragma once

#define FeaturePiece(_name_) \
    class _name_ final { \
public: \
    _name_() = delete; \
    static bool addComponentParts( \
            World &worldIn, RNG &rng, const BoundingBox &chunkBB, StructureComponent &piece); \
    }
