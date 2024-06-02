#pragma once

#include <cinttypes>
#include <cstdint>
#include <cstdlib>


///=============================================================================
///                      Compiler and Platform Features
///=============================================================================

#if __GNUC__
#define PREFETCH(PTR, RW, LOC) __builtin_prefetch(PTR, RW, LOC)
#define EXPECT_FALSE(COND) (__builtin_expect((COND), 0)) // [unlikely]
#define EXPECT_TRUE(COND) (__builtin_expect((COND), 1))  // [likely]
#define ATTR(...) __attribute__((__VA_ARGS__))
#else
#define PREFETCH(PTR, RW, LOC)
#define EXPECT_FALSE(COND) (COND) [[unlikely]]
#define EXPECT_TRUE(COND) (COND) [[likely]]
#define ATTR(...)
#endif

#define ND [[nodiscard]]
#define MU [[maybe_unused]]
