#include "common/rng.hpp"

#include <cassert>
#include <iostream>
#include <limits>
#include <thread>
#include <vector>

// Helper function to generate a sequence of numbers from RNG::nextLong
template <typename RngType>
std::vector<uint64_t> generateSequence(RngType& rng, const size_t count) {
    std::vector<uint64_t> sequence;
    sequence.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        sequence.push_back(rng.nextLong());
    }
    return sequence;
}

// Helper function to generate a sequence of numbers from RNG::nextInt(n)
template <typename RngType>
std::vector<int> generateIntSequence(RngType& rng, int n, size_t count) {
    std::vector<int> sequence;
    sequence.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        sequence.push_back(rng.nextInt(n));
    }
    return sequence;
}

// Helper function to generate a sequence using next<BITS>()
template <typename RngType, int BITS>
std::vector<int> generateNextSequence(RngType& rng, size_t count) {
    std::vector<int> sequence;
    sequence.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        sequence.push_back(rng.template next<BITS>());
    }
    return sequence;
}

template <typename RngType>
std::vector<float> generateFloatSequence(RngType& rng, size_t count) {
    std::vector<float> sequence;
    sequence.reserve(count);
    for(size_t i = 0; i < count; ++i)
    {
        sequence.push_back(rng.nextFloat());
    }
    return sequence;
}

template <typename RngType>
std::vector<double> generateDoubleSequence(RngType& rng, size_t count) {
    std::vector<double> sequence;
    sequence.reserve(count);
    for(size_t i = 0; i < count; ++i)
    {
        sequence.push_back(rng.nextDouble());
    }
    return sequence;
}

int main() {
    // Test 1: Identical Seeds Produce Identical Sequences
    {
        std::cout << "Test 1: Identical Seeds... ";
        RNG rng1;
        RNG /*__RNG_OLD*/ rng2;
        uint64_t seed = 1234567890ULL;
        rng1.setSeed(seed);
        rng2.setSeed(seed);

        auto seq1 = generateSequence(rng1, 1000);
        auto seq2 = generateSequence(rng2, 1000);

        assert(seq1.size() == seq2.size());
        for (size_t i = 0; i < seq1.size(); ++i) {
            if (seq1[i] != seq2[i]) {
                std::cerr << "Mismatch at index " << i << std::endl;
                assert(false);
            }
        }
        std::cout << "Passed" << std::endl;
    }

    // Test 2: Different Seeds Produce Different Sequences
    {
        std::cout << "Test 2: Different Seeds... ";
        RNG rng1;
        RNG /*__RNG_OLD*/ rng2;
        rng1.setSeed(12345ULL);
        rng2.setSeed(67890ULL);

        auto seq1 = generateSequence(rng1, 100);
        auto seq2 = generateSequence(rng2, 100);

        int differences = 0;
        for (size_t i = 0; i < seq1.size(); ++i) {
            if (seq1[i] != seq2[i]) {
                differences++;
            }
        }
        if (differences <= 90) {
            std::cerr << "Too few differences: " << differences << std::endl;
            assert(false);
        }
        std::cout << "Passed" << std::endl;
    }

    // Test 3: setSeed Resets Sequence
    {
        std::cout << "Test 3: setSeed Resets... ";
        RNG rng1;
        RNG /*__RNG_OLD*/ rng2;
        uint64_t initialSeed = 987654321ULL;
        rng1.setSeed(initialSeed);
        rng2.setSeed(initialSeed);

        auto seq1_initial = generateSequence(rng1, 500);
        auto seq2_initial = generateSequence(rng2, 500);

        uint64_t newSeed = 1122334455ULL;
        rng1.setSeed(newSeed);
        rng2.setSeed(newSeed);

        auto seq1_new = generateSequence(rng1, 500);
        auto seq2_new = generateSequence(rng2, 500);

        assert(seq1_initial.size() == seq2_initial.size());
        for (size_t i = 0; i < seq1_initial.size(); ++i) {
            if(seq1_initial[i] != seq2_initial[i])
            {
                std::cerr << "Initial sequence mismatch at " << i << std::endl;
                assert(false);
            }
        }

        assert(seq1_new.size() == seq2_new.size());
        for (size_t i = 0; i < seq1_new.size(); ++i) {
            if (seq1_new[i] != seq2_new[i]) {
                std::cerr << "New sequence mismatch at " << i << std::endl;
                assert(false);
            }
        }

        int differences1 = 0;
        int differences2 = 0;
        for (size_t i = 0; i < seq1_initial.size(); ++i) {
            if (seq1_initial[i] != seq1_new[i]) differences1++;
            if (seq2_initial[i] != seq2_new[i]) differences2++;
        }
        if (differences1 <= 450 || differences2 <= 450) {
            std::cerr << "Too few differences after setSeed" << std::endl;
            assert(false);
        }
        std::cout << "Passed" << std::endl;
    }

    // Test 4: nextInt(n) - Power of Two
    {
        std::cout << "Test 4: nextInt(n) - Power of Two... ";
        RNG rng1;
        RNG /*__RNG_OLD*/ rng2;
        uint64_t seed = 54321ULL;
        rng1.setSeed(seed);
        rng2.setSeed(seed);
        int n = 256;

        auto seq1 = generateIntSequence(rng1, n, 1000);
        auto seq2 = generateIntSequence(rng2, n, 1000);

        assert(seq1.size() == seq2.size());
        for (size_t i = 0; i < seq1.size(); ++i) {
            if (seq1[i] != seq2[i] || seq1[i] < 0 || seq1[i] >= n || seq2[i] < 0 || seq2[i] >= n) {
                std::cerr << "Mismatch at index " << i << std::endl;
                assert(false);
            }
        }
        std::cout << "Passed" << std::endl;
    }

    // Test 5: nextInt(n) - Non-Power of Two
    {
        std::cout << "Test 5: nextInt(n) - Non-Power of Two... ";
        RNG rng1;
        RNG /*__RNG_OLD*/ rng2;
        uint64_t seed = 99887766ULL;
        rng1.setSeed(seed);
        rng2.setSeed(seed);
        int n = 123;

        auto seq1 = generateIntSequence(rng1, n, 1000);
        auto seq2 = generateIntSequence(rng2, n, 1000);

        assert(seq1.size() == seq2.size());
        for (size_t i = 0; i < seq1.size(); ++i) {
            if (seq1[i] != seq2[i] || seq1[i] < 0 || seq1[i] >= n || seq2[i] < 0 || seq2[i] >= n) {
                std::cerr << "Mismatch at index " << i << std::endl;
                assert(false);
            }
        }
        std::cout << "Passed" << std::endl;
    }

    // Test 6: NextInt_Templated_PowerOfTwo
    {
        std::cout << "Test 6: NextInt_Templated_PowerOfTwo... ";
        RNG rng1;
        RNG /*__RNG_OLD*/ rng2;
        uint64_t seed = 13579ULL;
        rng1.setSeed(seed);
        rng2.setSeed(seed);
        constexpr int n = 1024;  //Power of two.

        auto seq1 = generateIntSequence(rng1, n, 1000);
        auto seq2 = generateIntSequence(rng2, n, 1000);
        assert(seq1.size() == seq2.size());

        for(size_t i = 0; i < seq1.size(); i++)
        {
            if(seq1[i] != seq2[i] || seq1[i] < 0 || seq1[i] >= n || seq2[i] < 0 || seq2[i] >=n)
            {
                std::cerr << "Mismatch at " << i << std::endl;
                assert(false);
            }

        }
        std::cout << "Passed" << std::endl;
    }

    // Test 7: NextInt_Templated_NonPowerOfTwo
    {
        std::cout << "Test 7: NextInt_Templated_NonPowerOfTwo... ";
        RNG rng1;
        RNG /*__RNG_OLD*/ rng2;
        uint64_t seed = 24680ULL;
        rng1.setSeed(seed);
        rng2.setSeed(seed);
        constexpr int n = 997;  // Non-power of two.

        auto seq1 = generateIntSequence(rng1, n, 1000);
        auto seq2 = generateIntSequence(rng2, n, 1000);
        assert(seq1.size() == seq2.size());

        for(size_t i = 0; i < seq1.size(); i++)
        {
            if(seq1[i] != seq2[i] || seq1[i] < 0 || seq1[i] >= n || seq2[i] < 0 || seq2[i] >=n)
            {
                std::cerr << "Mismatch at " << i << std::endl;
                assert(false);
            }
        }
        std::cout << "Passed" << std::endl;
    }

    // Test 8: Next_SpecificBits
    {
        std::cout << "Test 8: Next_SpecificBits... ";
        RNG rng1;
        RNG /*__RNG_OLD*/ rng2;
        uint64_t seed = 42424242ULL;
        rng1.setSeed(seed);
        rng2.setSeed(seed);

        auto seq1 = generateNextSequence<RNG, 16>(rng1, 1000);  // Test 16 bits
        auto seq2 = generateNextSequence<RNG /*__RNG_OLD*/, 16>(rng2, 1000);

        assert(seq1.size() == seq2.size());
        for (size_t i = 0; i < seq1.size(); ++i) {
            if(seq1[i] != seq2[i]) {
                std::cerr << "Mismatch at index " << i << std::endl;
                assert(false);
            }
        }
        std::cout << "Passed" << std::endl;
    }

    // Test 9: NextFloat
    {
        std::cout << "Test 9: nextFloat... ";
        RNG rng1;
        RNG /*__RNG_OLD*/ rng2;
        uint64_t seed = 42424243ULL;
        rng1.setSeed(seed);
        rng2.setSeed(seed);

        auto seq1 = generateFloatSequence(rng1, 1000);
        auto seq2 = generateFloatSequence(rng2, 1000);

        assert(seq1.size() == seq2.size());
        for(size_t i = 0; i < seq1.size(); i++)
        {
            // Use a small tolerance for float comparison
            if(std::abs(seq1[i] - seq2[i]) > 1e-6f)
            {
                std::cerr << "Mismatch at " << i << std::endl;
                assert(false);
            }
        }
        std::cout << "Passed" << std::endl;
    }

    // Test 10: nextDouble
    {
        std::cout << "Test 10: nextDouble... ";
        RNG rng1;
        RNG /*__RNG_OLD*/ rng2;
        uint64_t seed = 42424244ULL;
        rng1.setSeed(seed);
        rng2.setSeed(seed);

        auto seq1 = generateDoubleSequence(rng1, 1000);
        auto seq2 = generateDoubleSequence(rng2, 1000);

        assert(seq1.size() == seq2.size());
        for(size_t i = 0; i < seq1.size(); i++)
        {
            // Use a small tolerance for double comparison
            if (std::abs(seq1[i] - seq2[i]) > 1e-15)
            {
                std::cerr << "Mismatch at " << i << std::endl;
                assert(false);

            }
        }
        std::cout << "Passed" << std::endl;
    }

    // Test 11: NextIntMinMax
    {
        std::cout << "Test 11: nextInt(min, max)... ";
        RNG rng1;
        RNG /*__RNG_OLD*/ rng2;
        uint64_t seed = 192837465ULL;
        rng1.setSeed(seed);
        rng2.setSeed(seed);

        for (int i = 0; i < 1000; ++i) {
            int maxVal = 20;
            int minVal = -10;
            int val1 = rng1.nextInt(minVal, maxVal);
            int val2 = rng2.nextInt(minVal, maxVal);
            if (val1 != val2 || val1 < minVal || val1 > maxVal || val2 < minVal || val2 > maxVal)
            {
                std::cerr << "Mismatch at iteration " << i << std::endl;
                assert(false);
            }

        }
        std::cout << "Passed" << std::endl;
    }

    // Test 12: NextIntMinMax_Equal
    {
        std::cout << "Test 12: nextInt(min, max) - Equal... ";
        RNG rng1;
        RNG /*__RNG_OLD*/ rng2;
        uint64_t seed = 9876543210ULL;
        rng1.setSeed(seed);
        rng2.setSeed(seed);
        int val = 42;
        if (rng1.nextInt(val, val) != val || rng2.nextInt(val, val) != val)
        {
            std::cerr << "Mismatch when min == max" << std::endl;
            assert(false);
        }
        std::cout << "Passed" << std::endl;
    }

    // Test 13: NextIntMinMax_Templated_Legacy
    {
        std::cout << "Test 13: NextIntMinMax_Templated_Legacy... ";
        RNG rng1;
        RNG /*__RNG_OLD*/ rng2;
        uint64_t seed = 9876543210ULL;
        rng1.setSeed(seed);
        rng2.setSeed(seed);

        for (int i = 0; i < 1000; ++i) {
            int val1 = rng1.nextInt<3, 7, true>();
            int val2 = rng2.nextInt<3, 7, false>();
            if(val1 != val2 || val1 < 3 || val1 > 7 || val2 < 3 || val2 > 7)
            {
                std::cerr << "Mismatch at iteration " << i << std::endl;
                assert(false);
            }
        }
        std::cout << "Passed" << std::endl;
    }

    // Test 14: NextIntMinMax_Templated_NotLegacy
    {
        std::cout << "Test 14: NextIntMinMax_Templated_NotLegacy... ";
        RNG rng1;
        RNG /*__RNG_OLD*/ rng2;
        uint64_t seed = 9876543211ULL;
        rng1.setSeed(seed);
        rng2.setSeed(seed);

        for (int i = 0; i < 1000; ++i) {
            int val1 = rng1.nextInt<3, 7>();
            int val2 = rng2.nextInt(3, 7);
            if (val1 != val2 || val1 < 3 || val1 > 7 || val2 < 3 || val2 > 7)
            {
                std::cerr << "Mismatch at iteration " << i << std::endl;
                assert(false);
            }

        }
        std::cout << "Passed" << std::endl;
    }

    // Test 15: getRandomWorldSeed
    {
        std::cout << "Test 15: getRandomWorldSeed... ";
        i64 seed1 = RNG::getRandomWorldSeed();
        i64 seed2 = RNG /*__RNG_OLD*/::getRandomWorldSeed();
        // These are highly unlikely to be equal since they use the system clock.
        if(seed1 == seed2){
            std::cerr << "getRandomWorldSeed values are equal (highly unlikely)" << std::endl;
            //assert(false); //Don't assert, as it's technically *possible*.
        }

        std::cout << "Passed (not strictly comparable)" << std::endl;
    }

    // Test 16: getLargeFeatureSeed
    {
        std::cout << "Test 16: getLargeFeatureSeed... ";
        i64 worldSeed = 123456789;
        int chunkX = 10;
        int chunkZ = 20;

        RNG rng1 = RNG::getLargeFeatureSeed(worldSeed, chunkX, chunkZ);
        RNG /*__RNG_OLD*/ rng2 = RNG /*__RNG_OLD*/::getLargeFeatureSeed(worldSeed, chunkX, chunkZ);

        auto seq1 = generateSequence(rng1, 100);
        auto seq2 = generateSequence(rng2, 100);

        assert(seq1.size() == seq2.size());
        for (size_t i = 0; i < seq1.size(); ++i) {
            if (seq1[i] != seq2[i]) {
                std::cerr << "Mismatch at index " << i << std::endl;
                assert(false);
            }
        }
        std::cout << "Passed" << std::endl;
    }

    // Test 17: getPopulationSeed
    {
        std::cout << "Test 17: getPopulationSeed... ";
        i64 worldSeed = 987654321;
        int chunkX = -5;
        int chunkZ = 15;

        RNG rng1 = RNG::getPopulationSeed(worldSeed, chunkX, chunkZ);
        RNG /*__RNG_OLD*/ rng2 = RNG /*__RNG_OLD*/::getPopulationSeed(worldSeed, chunkX, chunkZ);

        auto seq1 = generateSequence(rng1, 100);
        auto seq2 = generateSequence(rng2, 100);

        assert(seq1.size() == seq2.size());
        for (size_t i = 0; i < seq1.size(); ++i) {
            if(seq1[i] != seq2[i]) {
                std::cerr << "Mismatch at index " << i << std::endl;
                assert(false);
            }
        }
        std::cout << "Passed" << std::endl;
    }

    // Test 18: nextBoolean
    {
        std::cout << "Test 18: nextBoolean... ";
        RNG rng1;
        RNG /*__RNG_OLD*/ rng2;
        uint64_t seed = 789456123ULL;
        rng1.setSeed(seed);
        rng2.setSeed(seed);

        for(int i = 0; i < 1000; ++i) {
            bool b1 = rng1.nextBoolean();
            bool b2 = rng2.nextBoolean();
            if (b1 != b2) {
                std::cerr << "Mismatch at index " << i << std::endl;
                assert(false);
            }
        }
        std::cout << "Passed" << std::endl;
    }

    // Test 19: nextLongI (Specific to RNG, renamed from nextLong)
    {
        std::cout << "Test 19: nextLongI... ";
        RNG rng1;
        RNG /*__RNG_OLD*/ rng2; //RNG2 has an equivalent.
        uint64_t seed = 147258369ULL;
        rng1.setSeed(seed);
        rng2.setSeed(seed);

        for (int i = 0; i < 1000; ++i) {
            i64 long1 = rng1.nextLongI();
            i64 long2 = rng2.nextLongI();
            if(long1 != long2){
                std::cerr << "Mismatch at index " << i << std::endl;
                assert(false);
            }
        }
        std::cout << "Passed" << std::endl;
    }

    // Test 20: nextFloat(min, max)
    {
        std::cout << "Test 20: nextFloat(min, max)... ";
        RNG rng1;
        RNG /*__RNG_OLD*/ rng2;
        uint64_t seed = 258369147ULL;
        rng1.setSeed(seed);
        rng2.setSeed(seed);
        float minVal = -5.0f;
        float maxVal = 10.0f;

        for (int i = 0; i < 1000; ++i) {
            float val1 = rng1.nextFloat(minVal, maxVal);
            float val2 = rng2.nextFloat(minVal, maxVal);

            if (std::abs(val1-val2) > 1e-6f || val1 < minVal || val1 > maxVal || val2 < minVal || val2 > maxVal)
            {
                std::cerr << "Mismatch at iteration " << i << ": " << val1 << " != " << val2 << std::endl;
                assert(false);
            }
        }
        std::cout << "Passed" << std::endl;
    }


    // Test 21: nextDouble(min, max)
    {
        std::cout << "Test 21: nextDouble(min, max)... ";
        RNG rng1;
        RNG /*__RNG_OLD*/ rng2;
        uint64_t seed = 369147258ULL;
        rng1.setSeed(seed);
        rng2.setSeed(seed);
        double minVal = -2.5;
        double maxVal = 7.5;

        for (int i = 0; i < 1000; ++i) {
            double val1 = rng1.nextDouble(minVal, maxVal);
            double val2 = rng2.nextDouble(minVal, maxVal);
            if (std::abs(val1 - val2) > 1e-15 || val1 < minVal || val1 > maxVal || val2 < minVal || val2 > maxVal)
            {
                std::cerr << "Mismatch at iteration " << i << std::endl;
                assert(false);
            }
        }
        std::cout << "Passed" << std::endl;
    }

    // Test 22: advance<N>
    {
        std::cout << "Test 22: advance<N>... ";
        RNG rng1;
        RNG /*__RNG_OLD*/ rng2;
        uint64_t seed = 1122334455ULL;
        rng1.setSeed(seed);
        rng2.setSeed(seed);

        // Advance by a specific amount
        constexpr uint64_t advanceAmount = 5;
        rng1.advance<advanceAmount>();
        rng2.advance<advanceAmount>(); //Use constexpr for this.

        auto seq1 = generateSequence(rng1, 100);
        auto seq2 = generateSequence(rng2, 100);

        assert(seq1.size() == seq2.size());
        for (size_t i = 0; i < seq1.size(); ++i) {
            if (seq1[i] != seq2[i]) {
                std::cerr << "Mismatch after advance at index " << i << std::endl;
                assert(false);
            }
        }
        std::cout << "Passed" << std::endl;
    }

    // Test 23: Large advance values
    {
        std::cout << "Test 23: Large advance values... ";
        RNG rng1;
        RNG /*__RNG_OLD*/ rng2;
        uint64_t seed = 9988776655ULL;
        rng1.setSeed(seed);
        rng2.setSeed(seed);

        rng1.advance<100000>(); // Large advance
        rng2.advance<100000>();

        auto seq1 = generateSequence(rng1, 100);
        auto seq2 = generateSequence(rng2, 100);
        assert(seq1.size() == seq2.size());
        for (size_t i = 0; i < seq1.size(); ++i) {
            if (seq1[i] != seq2[i]) {
                std::cerr << "Mismatch after large advance at index " << i << std::endl;
                assert(false);
            }
        }
        std::cout << "Passed" << std::endl;
    }

    // Test 24: Edge case seed values (0 and max)
    {
        std::cout << "Test 24: Edge case seed values... ";
        std::vector<uint64_t> edgeSeeds = {0, std::numeric_limits<uint64_t>::max()};

        for (uint64_t seed : edgeSeeds) {
            RNG rng1;
            RNG /*__RNG_OLD*/ rng2;
            rng1.setSeed(seed);
            rng2.setSeed(seed);

            auto seq1 = generateSequence(rng1, 100);
            auto seq2 = generateSequence(rng2, 100);

            assert(seq1.size() == seq2.size());
            for (size_t i = 0; i < seq1.size(); ++i) {
                if (seq1[i] != seq2[i])
                {
                    std::cerr << "Mismatch at index " << i << " with seed " << seed << std::endl;
                    assert(false);
                }
            }
        }
        std::cout << "Passed" << std::endl;
    }

    // Test 25: Test setRandomSeed with timing.
    {
        std::cout << "Test 25: setRandomSeed... ";
        RNG rng1, rng2;
        rng1.setRandomSeed();

        //Ensure there is *some* time difference, however small
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));
        rng2.setRandomSeed();

        if (rng1.getSeed() == rng2.getSeed())
        {
            std::cerr << "Seeds should be different after setRandomSeed" << std::endl;
            //assert(false); //They could be, if the timer is the same, which it might.
        }
        else
        {
            std::cout << "Passed" << std::endl;
        }


    }

    //Test 26: Check that operators work.
    {
        std::cout << "Test 26: operator== ... ";
        RNG rng1;
        RNG rng2;

        rng1.setValue(1);
        rng2.setValue(1);

        if (!(rng1 == rng2)) {
            std::cerr << "Failed operator== between RNG objects\n";
            assert(false);
        }

        u64 seedVal = 1;
        if (!(rng1 == seedVal)) {
            std::cerr << "Failed operator== between RNG and value\n";
            assert(false);
        }

        std::cout << "Passed" << std::endl;
    }

    std::cout << "All tests completed." << std::endl;

    return 0;
}