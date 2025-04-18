#pragma once

/**
 * @file Roll.hpp
 * @brief Provides a compile-time random roll generator.
 *
 * This file defines the template class @c Roll, which encapsulates a fixed range of integer
 * values (from @c MinRoll to @c MaxRoll) and provides static methods to retrieve these bounds
 * and generate a random value within this range.
 */

#include "common/rng.hpp"
#include "lce/processor.hpp"

namespace loot {

    /**
     * @brief Compile-time random roll generator.
     *
     * The @c Roll class uses the template parameters @c MinRoll and @c MaxRoll to define
     * a range and provides functions to obtain the bounds and generate a random value in that range.
     *
     * @tparam MinRoll The minimum roll value (inclusive).
     * @tparam MaxRoll The maximum roll value (inclusive).
     */
    template<u8 MinRoll, u8 MaxRoll>
    class Roll final {
    public:
        /**
         * @brief Default constructor.
         *
         * This constructor is evaluated at compile time and marked as <c>noexcept</c>.
         */
        consteval Roll() noexcept = default;

        /**
         * @brief Returns the minimum roll value.
         *
         * @return The compile-time constant <c>MinRoll</c>.
         */
        ND static constexpr u8 getMin() noexcept { return MinRoll; }

        /**
         * @brief Returns the maximum roll value.
         *
         * @return The compile-time constant <c>MaxRoll</c>.
         */
        ND static constexpr u8 getMax() noexcept { return MaxRoll; }

        /**
         * @brief Generates a random value within the range [@c MinRoll, @c MaxRoll].
         *
         * @tparam Legacy A flag to select between RNG modes.
         * @param rng A reference to an RNG instance (assumed to implement <c>nextInt</c>).
         * @return A random integer between <c>MinRoll</c> and <c>MaxRoll</c>.
         */
        template<bool Legacy>
        static constexpr u8 rollValue(RNG &rng) noexcept {
            return rng.nextInt<MinRoll, MaxRoll, Legacy>();
        }

        /**
         * @brief Outputs the roll range as a formatted string.
         *
         * The output is formatted as "Roll(MinRoll, MaxRoll)".
         *
         * @param out The output stream.
         * @param roll A Roll instance (unused because values are compile-time constants).
         * @return A reference to the output stream.
         */
        friend std::ostream &operator<<(std::ostream &out, MU const Roll &roll) {
            out << "Roll(" << static_cast<int>(MinRoll) << ", " << static_cast<int>(MaxRoll) << ")";
            return out;
        }
    };

}// namespace loot
