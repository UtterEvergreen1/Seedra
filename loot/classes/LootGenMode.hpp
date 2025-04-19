#pragma once

/**
 * @file LootGenMode.hpp
 * @brief Defines the loot generation mode enumeration.
 *
 * @details
 * The @c GenMode enum specifies the mode used during loot generation, which controls both
 * the random number generator (RNG) behavior and the shuffling of loot items.
 *
 * The available modes are:
 * - @c LEGACY: Uses legacy RNG behavior (places items at random indexes).
 * - @c MODERN: Uses modern RNG behavior with a modern shuffle algorithm.
 * - @c MOD_NO_SHUF: Uses modern RNG behavior but creates the list of items incrementally without shuffling after loot generation.
 */

namespace loot {
    enum class GenMode {
        LEGACY,     ///< Legacy RNG behavior (places items at random indexes).
        MODERN,     ///< Modern RNG behavior (uses modern shuffle algorithm).
        MOD_NO_SHUF ///< Modern RNG behavior (creates a list of items incrementally; does not shuffle after generating the loot).
    };
}
