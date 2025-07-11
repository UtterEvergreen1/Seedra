#pragma once

/**
 * @file LootTable.hpp
 * @brief Defines the LootTable class and related utilities for loot generation.
 *
 * @details
 * This file implements a compile-time configurable loot table for generating loot items.
 * It includes:
 * - The @c LootTableType enum for categorizing loot tables.
 * - The @c setTableType() function to determine the table type based on the roll range.
 * - The @c LootTable class template that encapsulates an array of loot item variants,
 *   cumulative weight computation, and functions to generate loot items.
 * - The @c TableWrapper structure to simplify creation of a LootTable from multiple loot items.
 */

#include "Roll.hpp"
#include "Container.hpp"
#include "LootItem.hpp"
#include "lce/processor.hpp"

namespace loot {

    /**
     * @brief Enumeration of loot table types.
     *
     * @details
     * The @c LootTableType enum categorizes loot tables based on the roll range and number of items:
     * - @c RETURNS_1: Table returns exactly one item (<c>Roll(1,1)</c>).
     * - @c RETURNS_N: Table returns a fixed number of items (<c>Roll(N,N)</c>, with N ≠ 1).
     * - @c RETURNS_R: Table returns a random number of items (<c>Roll(MinRoll, MaxRoll)</c> with MinRoll ≠ MaxRoll).
     */
    enum class LootTableType {
        RETURNS_1, ///< Table returns exactly one item.
        RETURNS_N, ///< Table returns a fixed number of items.
        RETURNS_R  ///< Table returns a random number of items.
    };

    /**
     * @brief Determines the loot table type based on roll parameters.
     *
     * @tparam MinRoll The minimum roll value.
     * @tparam MaxRoll The maximum roll value.
     * @return A @c LootTableType value that classifies the table.
     *
     * @details
     * - Returns @c RETURNS_1 if both @c MinRoll and @c MaxRoll equal 1.
     * - Returns @c RETURNS_N if @c MinRoll equals @c MaxRoll (and is not 1).
     * - Returns @c RETURNS_R otherwise.
     */
    template<u8 MinRoll, u8 MaxRoll>
    static consteval LootTableType setTableType() {
        if constexpr (MinRoll == 1 && MaxRoll == 1) {
            return LootTableType::RETURNS_1;
        } else if constexpr (MinRoll == MaxRoll) {
            return LootTableType::RETURNS_N;
        } else {
            return LootTableType::RETURNS_R;
        }
    }

    /**
     * @brief Template class representing a loot table.
     *
     * @tparam N The number of loot items in the table.
     * @tparam MinRoll The minimum roll value for item generation.
     * @tparam MaxRoll The maximum roll value for item generation.
     * @tparam LootItemVariant_t The variant type that holds various @c LootItem types.
     *
     * @details
     * The @c LootTable class stores an array of loot items (as a variant) and computes their cumulative
     * weights at compile time. It uses a roll generator (@c Roll) to determine the number of items to generate.
     * The class provides functions to retrieve the total weight, maximum item count, and to generate loot
     * based on a random weight selection.
     */
    template<i32 N, u8 MinRoll, u8 MaxRoll, typename LootItemVariant_t>
    class MU LootTable final {
        using weight_array_t = std::array<u16, N>;

        // TODO: can I make the const fields static constexpr?

        /// Array of loot item variants.
        const std::array<LootItemVariant_t, N> m_Items;

        /// Array of cumulative weights for the loot items.
        const weight_array_t m_CumulativeWeights;

        /// Roll generator for determining the number of items to generate.
        const Roll<MinRoll, MaxRoll> m_Roll;

    public:
        /// The loot table type, computed from @c MinRoll and @c MaxRoll.
        MU static constexpr LootTableType k_TableType = setTableType<MinRoll, MaxRoll>();

        /// The number of loot items in the table.
        MU static constexpr u8 k_Size = N;

        /// The minimum roll value.
        MU static constexpr u8 k_MinRoll = MinRoll;

        /// The maximum roll value.
        MU static constexpr u8 k_MaxRoll = MaxRoll;

        /**
         * @brief Deleted default constructor.
         */
        consteval LootTable() = delete;

        /**
         * @brief Constructs a LootTable with the given loot items.
         *
         * @param LootItems An array of loot item variants.
         */
        consteval explicit LootTable(const std::array<LootItemVariant_t, N> LootItems) noexcept
            : m_Items(LootItems),
              m_CumulativeWeights(computeCumulativeWeights(LootItems)) {}

        DELETE_NEW_OPS;

        /**
         * @brief Checks if the loot table contains only one item.
         *
         * @return @c true if the table has exactly one item; otherwise, @c false.
         */
        static consteval bool hasOneItem() noexcept { return N == 1; }

        /**
         * @brief Retrieves the total cumulative weight of the loot table.
         *
         * @return The cumulative weight of the last loot item.
         */
        MU ND constexpr u16 totalWeight() const noexcept {
            return m_CumulativeWeights[N - 1];
        }

        /**
         * @brief Retrieves the maximum number of items that can be generated.
         *
         * @return The maximum roll value from the roll generator.
         */
        MU ND constexpr i32 maxItemCount() const noexcept {
            return m_Roll.getMax();
        }

        /**
         * @brief Generates the roll count using the provided RNG.
         *
         * @tparam Legacy A flag indicating whether to use legacy RNG behavior.
         * @param rng A reference to an RNG instance.
         * @return The number of rolls generated.
         */
        template<bool Legacy>
        MU i32 getRollCount(RNG &rng) const noexcept {
            const i32 rollCount = m_Roll.template rollValue<Legacy>(rng);
            return rollCount;
        }

        /**
         * @brief Selects and generates a loot item based on a random weight.
         *
         * @tparam ContainerSize The size of the container to store the generated item.
         * @tparam Legacy A flag indicating whether to use legacy RNG behavior.
         * @param container A reference to the container for the generated item.
         * @param rng A reference to an RNG instance used for random selection.
         * @return A @c lce::ItemState representing the generated loot item.
         *
         * @details
         * This function first generates a random weight by calling @c rng.nextInt(totalWeight()).
         * If the table contains only one item, the function directly generates the item state.
         * Otherwise, it performs a binary search over the cumulative weights to select the appropriate item.
         */
        template<int ContainerSize, bool Legacy>
        MU lce::ItemState rollLootItem(MU Container<ContainerSize> &container, RNG &rng) const {

            static constexpr bool TableContainsOneItem = hasOneItem();

            const volatile i32 randomWeight = rng.nextInt(totalWeight());

            if constexpr (TableContainsOneItem) {
                // Directly generate the item state if only one item is present.
                return std::visit([&](auto const &concreteItem) -> lce::ItemState {
                    return concreteItem.template generateItemState<ContainerSize, Legacy>(container, rng);
                },
                                  std::get<0>(m_Items));
            } else {
                const u64 index = findItemIndex(randomWeight);
                const auto item = getItem(index);

                return std::visit([&](auto const &concreteItem) -> lce::ItemState {
                    return concreteItem.template generateItemState<ContainerSize, Legacy>(container, rng);
                },
                                  item);
            }
        }

    private:


        /**
         * @brief Retrieves a loot item variant by its index.
         *
         * @param index The index of the loot item.
         * @return A reference to the loot item variant.
         */
        ND const LootItemVariant_t &getItem(size_t index) const noexcept {
            return m_Items[index];
        }

        /**
         * @brief Extracts the weight from a loot item variant.
         *
         * @tparam Variant The type of the loot item variant.
         * @param v The loot item variant.
         * @return The weight of the loot item.
         *
         * @note This function uses a lambda with the <c>std::visit</c> mechanism.
         */
        template<typename Variant>
        constexpr u16 getWeightFromVariant(const Variant &v) noexcept {
            return std::visit([]<typename LootItem_t>(MU LootItem_t const &item) -> u16 {
                return std::decay_t<LootItem_t>::k_Weight;
            },
                              v);
        }

        /**
         * @brief Computes the cumulative weights for the loot items.
         *
         * @param items An array of loot item variants.
         * @return An array of cumulative weights.
         *
         * @details
         * The function iterates over each loot item variant, extracts its weight, and computes a cumulative
         * sum. This array is later used for selecting items via binary search.
         */
        consteval weight_array_t computeCumulativeWeights(std::array<LootItemVariant_t, N> items) noexcept {
            weight_array_t cumulWeights;
            u16 totalWeight = 0;
            for (size_t i = 0; i < items.size(); ++i) {
                const u16 weight = getWeightFromVariant(items[i]);
                totalWeight += weight;
                cumulWeights[i] = totalWeight;
            }
            return cumulWeights;
        }

        /**
         * @brief Finds the index of the loot item corresponding to a given weight.
         *
         * @param theWeight A random weight used for selection.
         * @return The index of the selected loot item.
         *
         * @details
         * A binary search is performed over the cumulative weight array to determine the loot item
         * that should be selected based on @c theWeight.
         */
        MU ND u64 findItemIndex(const i32 theWeight) const {
            u64 low = 0;
            u64 high = N;
            while (low < high) {
                const u64 mid = (low + high) >> 1;
                if (m_CumulativeWeights[mid] > theWeight) {
                    high = mid;
                } else {
                    low = mid + 1;
                }
            }
            return low;
        }
    };

    /**
     * @brief Helper structure to create a LootTable from a list of LootItem types.
     *
     * @tparam MinRoll The minimum roll value for the table.
     * @tparam MaxRoll The maximum roll value for the table.
     * @tparam Items One or more @c LootItem types.
     *
     * @details
     * The @c TableWrapper structure automates the creation of a @c LootTable by determining the table
     * size from the number of provided @c LootItem types and constructing a variant type that can hold
     * any of the specified items.
     */
    template<int MinRoll, int MaxRoll, typename... Items>
    struct TableWrapper {
        using variant_t = LootItemVariant<Items...>;
        using table_t = LootTable<sizeof...(Items), MinRoll, MaxRoll, variant_t>;
        /// A compile-time constant instance of the loot table.
        static constexpr table_t value{
                std::array<variant_t, sizeof...(Items)>{Items()...}};
    };

} // namespace loot
