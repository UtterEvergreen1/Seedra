#pragma once

#include "common/Pos3DTemplate.hpp"
#include "lce/blocks/block.hpp"

template <typename T>
concept BlockAccess = requires(T t, const T c_t, const Pos3D& p, i32 x, i32 y, i32 z,
                               lce::BlockState b, u16 i, u8 d) {

    // GETTERS

    { t.getBlock(x, y, z) } -> std::same_as<lce::BlockState>;
    { t.getBlock(p)       } -> std::same_as<lce::BlockState>;

    { t.getBlockId(x, y, z) } -> std::same_as<u16>;
    { t.getBlockId(p)       } -> std::same_as<u16>;

    // SETTERS

    { t.setBlock(x, y, z, b) } -> std::same_as<void>;
    { t.setBlock(p, b)       } -> std::same_as<void>;

    { t.setBlockAndData(x, y, z, i, d) } -> std::same_as<void>;
    { t.setBlockAndData(p, i, d)       } -> std::same_as<void>;

    { t.setBlockId(x, y, z, i) } -> std::same_as<void>;
    { t.setBlockId(p, i)       } -> std::same_as<void>;

    // MUTATORS

    { t.mutBlockPtr(x, y, z)   } -> std::same_as<u16*>;


};
