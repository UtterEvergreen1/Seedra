#pragma once

#include "lce/processor.hpp"
#include "StringHash.hpp"

#include <iostream>
#include <chrono>

///=============================================================================
///                    C++ implementation of Java Random
///=============================================================================

/**
 * @class RNG
 * @brief A class that implements a random number generator similar to Java's Random class.
 */
class RNG {
    u64 seed = 0; ///< The internal seed value.

public:
    /**
     * @brief Default constructor.
     */
    RNG() = default;

    /**
     * @brief Constructor that initializes the RNG with a given seed.
     * @param seedIn The seed value.
     */
    explicit RNG(c_u64 seedIn) { this->setSeed(seedIn); }

    /**
     * @brief Constructor that initializes the RNG with a hashed string seed.
     * @param seedIn The string to hash and use as the seed.
     */
    explicit RNG(const std::string &seedIn) { this->setSeed(StringHash::hash(seedIn)); }

    /**
     * @brief Equality operator to compare two RNG objects.
     * @param other The other RNG object.
     * @return True if the seeds are equal, false otherwise.
     */
    bool operator==(const RNG &other) const { return seed == other.seed; }

    /**
     * @brief Equality operator to compare the RNG seed with a value.
     * @param rngValue The value to compare.
     * @return True if the seed matches the value, false otherwise.
     */
    bool operator==(c_u64 &rngValue) const { return seed == rngValue; }

    /**
     * @brief Creates a copy of the current RNG object.
     * @return A new RNG object with the same seed.
     */
    ND RNG copy() const {
        c_auto rng = RNG(seed);
        return rng;
    }

    /**
     * @brief Constructs an RNG object without setting the seed.
     * @param seedIn The seed value.
     * @return A new RNG object with the specified seed.
     */
    static RNG ConstructWithoutSetSeed(c_u64 seedIn) {
        RNG rng;
        rng.seed = seedIn;
        return rng;
    }

    /**
     * @brief Generates a random world seed.
     * @return A random 64-bit integer seed.
     */
    MU static int64_t getRandomWorldSeed() {
        RNG rng = initializeWithRandomSeed();
        return rng.nextLongI();
    }

    /**
     * @brief Generates a seed for large feature generation.
     * @param worldSeed The world seed.
     * @param chunkX The x-coordinate of the chunk.
     * @param chunkZ The z-coordinate of the chunk.
     * @return A new RNG object initialized with the large feature seed.
     */
    static RNG getLargeFeatureSeed(c_i64 worldSeed, c_int chunkX, c_int chunkZ) {
        RNG rng;
        rng.setSeed(worldSeed);
        c_auto l2 = (i64) rng.nextLong();
        c_auto l3 = (i64) rng.nextLong();
        c_i64 l4 = (i64) chunkX * l2 ^ (i64) chunkZ * l3 ^ worldSeed;
        rng.setSeed(l4);
        return rng;
    }

    /**
     * @brief Generates a seed for population generation.
     * @param worldSeed The world seed.
     * @param chunkX The x-coordinate of the chunk.
     * @param chunkZ The z-coordinate of the chunk.
     * @return A new RNG object initialized with the population seed.
     */
    static RNG getPopulationSeed(c_i64 worldSeed, c_int chunkX, c_int chunkZ) {
        RNG rng;
        rng.setSeed(worldSeed);
        auto a = (i64) rng.nextLong();
        auto b = (i64) rng.nextLong();
        a = ((a / 2) * 2) + 1;
        b = ((b / 2) * 2) + 1;
        c_i64 decoratorSeed = (chunkX * a + chunkZ * b) ^ worldSeed;
        rng.setSeed(decoratorSeed);
        return rng;
    }

    /**
     * @brief Initializes an RNG object with a random seed.
     * @return A new RNG object with a random seed.
     */
    static RNG initializeWithRandomSeed() {
        RNG rng{};
        rng.setRandomSeed();
        return rng;
    }

    /**
     * @brief Sets the RNG seed to a random value based on the current time.
     */
    MU void setRandomSeed() {
        this->setSeed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    }

    static constexpr uint64_t mask = (1ULL << 48) - 1; ///< Mask for 48-bit seed.

    /**
     * @brief Computes the multiplier for advancing the RNG state.
     * @param amount The number of steps to advance.
     * @return The computed multiplier.
     */
    static constexpr uint64_t computeMultiplier(const uint64_t amount) {
        uint64_t m = 1;
        uint64_t im = 0x5deece66dULL;
        for (uint64_t k = amount; k; k >>= 1) {
            if (k & 1) {
                m *= im;
            }
            im *= im;
        }
        return m & mask;
    }

    /**
     * @brief Computes the addend for advancing the RNG state.
     * @param amount The number of steps to advance.
     * @return The computed addend.
     */
    static constexpr uint64_t computeAddend(const uint64_t amount) {
        uint64_t a = 0;
        uint64_t im = 0x5deece66dULL;
        uint64_t ia = 0xb;
        for (uint64_t k = amount; k; k >>= 1) {
            if (k & 1) {
                a = im * a + ia;
            }
            ia = (im + 1) * ia;
            im *= im;
        }
        return a & mask;
    }

    /**
     * @brief Advances the RNG state by a specified number of steps.
     * @tparam amount The number of steps to advance (default is 1).
     */
    template<uint64_t amount = 1>
    void advance() {
        constexpr uint64_t multiplier = computeMultiplier(amount);
        constexpr uint64_t addend = computeAddend(amount);
        seed = (seed * multiplier + addend) & mask;
    }

    /**
     * @brief Gets the current seed value.
     * @return The current seed.
     */
    ND MU u64 getSeed() const { return seed; }

    /**
     * @brief Sets the RNG seed to a specific value.
     * @param value The new seed value.
     */
    MU void setValue(c_u64 value) { seed = value; }

    /**
     * @brief Sets the RNG seed with a specific value, applying a transformation.
     * @param value The new seed value.
     */
    void setSeed(c_u64 value) { seed = (value ^ 0x5deece66d) & 0xFFFFFFFFFFFF; }

    /**
     * @brief Generates the next random number with a specified number of bits.
     * @param bits The number of bits.
     * @return The generated random number.
     */
    int next(c_int bits) {
        advance();
        return (int) ((i64) seed >> (48 - bits));
    }

    /**
     * @brief Generates a random boolean value.
     * @return True or false.
     */
    bool nextBoolean() { return next(1) != 0; }

    /**
     * @brief Generates a random integer.
     * @return The generated integer.
     */
    int nextInt() { return next(32); }

    /**
     * @brief Generates a random integer within a range.
     * @param n The upper bound (exclusive).
     * @return The generated integer.
     */
    int nextInt(c_int n) {
        int bits, val;
        c_int m = n - 1;

        if ((m & n) == 0) {
            u64 x = n * (u64) next(31);
            return (int) ((i64) x >> 31);
        }

        do {
            bits = next(31);
            val = bits % n;
        } while (bits - val + m < 0);
        return val;
    }

    /**
     * @brief Generates a random integer within a specified range.
     * @param minimum The lower bound (inclusive).
     * @param maximum The upper bound (inclusive).
     * @return The generated integer.
     */
    int nextInt(c_int minimum, c_int maximum) {
        return minimum >= maximum ? minimum : nextInt(maximum - minimum + 1) + minimum;
    }

    /**
     * @brief Generates a random integer within a specified range using a legacy implementation.
     *
     * This method calculates a random integer between the given minimum and maximum values (inclusive).
     * It uses the `nextInt` method to generate the random number.
     *
     * @param minimum The lower bound of the range (inclusive).
     * @param maximum The upper bound of the range (inclusive).
     * @return A random integer between the minimum and maximum values.
     */
    int nextIntLegacy(c_int minimum, c_int maximum) { return nextInt(maximum - minimum + 1) + minimum; }

    /**
     * @brief Generates a random long value.
     * @return The generated long value.
     */
    u64 nextLong() { return ((u64) next(32) << 32) + next(32); }

    /**
     * @brief Generates a random signed long value.
     * @return The generated signed long value.
     */
    i64 nextLongI() { return static_cast<i64>((static_cast<u64>(next(32)) << 32) + next(32)); }

    /**
     * @brief Generates a random float value between 0.0 and 1.0.
     * @return The generated float value.
     */
    float nextFloat() { return (float) next(24) / (float) 0x1000000; }

    /**
     * @brief Generates a random float value within a specified range.
     * @param minimum The lower bound (inclusive).
     * @param maximum The upper bound (inclusive).
     * @return The generated float value.
     */
    MU float nextFloat(const float minimum, const float maximum) {
        return minimum >= maximum ? minimum : nextFloat() * (maximum - minimum) + minimum;
    }

    /**
     * @brief Generates a random double value between 0.0 and 1.0.
     * @return The generated double value.
     */
    double nextDouble() {
        u64 x = next(26);
        x <<= 27;
        x += next(27);
        return (i64) x / (double) 0x20000000000000;
    }

    /**
     * @brief Generates a random double value within a specified range.
     * @param minimum The lower bound (inclusive).
     * @param maximum The upper bound (inclusive).
     * @return The generated double value.
     */
    MU double nextDouble(c_double minimum, c_double maximum) {
        return minimum >= maximum ? minimum : nextDouble() * (maximum - minimum) + minimum;
    }

    /**
     * @brief Skips a specified number of random number generations.
     * @param n The number of random numbers to skip.
     */
    MU void skipNextN(c_u64 n) {
        u64 m = 1;
        u64 a = 0;
        u64 im = 0x5deece66d;
        u64 ia = 0xb;
        for (u64 k = n; k; k >>= 1) {
            if (k & 1) {
                m *= im;
                a = im * a + ia;
            }
            ia = (im + 1) * ia;
            im *= im;
        }
        seed = seed * m + a;
        seed &= 0xFFFFFFFFFFFF;
    }

    /**
     * @brief Generates a random Gaussian value (not implemented).
     * @return A placeholder value (0.0).
     */
    MU static double nextGaussian() {
        return 0.0;
    }

    /**
     * @brief Generates a random Gaussian float value.
     * @return The generated Gaussian float value.
     */
    MU double nextGaussianFloat() {
        c_double dVar1 = nextFloat();
        c_double dVar2 = nextFloat();
        return static_cast<float>(dVar1 - dVar2);
    }

    /**
     * @brief Generates a random Gaussian integer value.
     * @param param2 A parameter for the Gaussian generation.
     * @return The generated Gaussian integer value.
     */
    MU int nextGaussianInt(c_int param2) {
        c_int iVar1 = nextInt();
        c_int iVar2 = nextInt(param2);
        return iVar1 - iVar2;
    }
};
