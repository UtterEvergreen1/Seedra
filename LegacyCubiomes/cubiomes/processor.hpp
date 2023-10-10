#pragma once

#include <cstdlib>
#include <cstdint>
#include <cinttypes>


///=============================================================================
///                      Compiler and Platform Features
///=============================================================================

#if __GNUC__

#define ND                      [[nodiscard]]
#define MU                      [[maybe_unused]]


#define PREFETCH(PTR,RW,LOC)                __builtin_prefetch(PTR,RW,LOC)
#define EXPECT_FALSE(COND)                 (__builtin_expect((COND),0))    // [[unlikely]
#define EXPECT_TRUE(COND)                 (__builtin_expect((COND),1))    // [[likely]
#define ATTR(...)               __attribute__((__VA_ARGS__))
#else
#define ND                      [[nodiscard]]
#define MU                      [[maybe_unused]]

#define PREFETCH(PTR,RW,LOC)
#define EXPECT_FALSE(COND)                (COND) [[unlikely]]
#define EXPECT_TRUE(COND)                 (COND) [[likely]]
#define ATTR(...)
#endif

/// imitate amd64/x64 rotate instructions

MU static inline ATTR(const, always_inline, artificial)
uint64_t rotl64(uint64_t x, uint8_t b)
{
    return (x << b) | (x >> (64-b));
}

MU static inline ATTR(const, always_inline, artificial)
uint32_t rotr32(uint32_t a, uint8_t b)
{
    return (a >> b) | (a << (32-b));
}
