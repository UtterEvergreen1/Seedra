#pragma once

/**
 * @file LootItem.hpp
 * @brief Defines the LootItem template class for representing loot items.
 *
 * @details
 * The @c LootItem class template encapsulates the properties of a loot item, including its weight,
 * roll range (from @c MinRoll to @c MaxRoll), a default item state, and an optional function to modify
 * the generated item state. These parameters are set at compile time, allowing for optimizations and
 * static checking. This class is used to generate an @c lce::ItemState based on a random roll and
 * optional modifications.
 */

#include "Roll.hpp"
#include "LootFunctions.hpp"
#include "lce/processor.hpp"
#include "lce/items/Item.hpp"

namespace loot {

    /**
     * @brief Represents a loot item with compile-time parameters.
     *
     * The @c LootItem class template defines a loot item by specifying:
     * - A weight (@c Weight) that influences its selection probability.
     * - A roll range, defined by @c MinRoll and @c MaxRoll, that determines the number of items generated.
     * - A default state (@c TheState) that is used as the basis for the generated item.
     * - An optional function (@c Func) to apply additional modifications (such as metadata or enchantments)
     *   to the item state during generation.
     *
     * @tparam Weight The weight of the loot item.
     * @tparam MinRoll The minimum number of items generated (inclusive).
     * @tparam MaxRoll The maximum number of items generated (inclusive). Defaults to @c MinRoll.
     * @tparam TheState The default @c lce::ItemState for this loot item. Defaults to <c>lce::ItemState(0, 0)</c>.
     * @tparam Func An optional function pointer of type @c LootApplyFunc to modify the item state.
     *
     * @note This class is intended for compile-time configuration and optimization.
     */
    template<u16 Weight, u8 MinRoll, u8 MaxRoll = MinRoll,
             lce::ItemState TheState = lce::ItemState(0, 0), LootApplyFunc Func = nullptr>
    class MU LootItem {
        using RollType = Roll<MinRoll, MaxRoll>;

        /// Optional function to modify the generated item state.
        static constexpr LootApplyFunc k_Func = Func;

        /// Default item state used for this loot item.
        static constexpr lce::ItemState k_DefaultItemState = TheState;

        /// Roll generator for this loot item.
        static constexpr RollType k_Roll = RollType();

    public:
        /// The minimum roll value.
        MU static constexpr u8 k_MinRoll = MinRoll;

        /// The maximum roll value.
        MU static constexpr u8 k_MaxRoll = MaxRoll;

        /// The weight of the loot item.
        MU static constexpr u16 k_Weight = Weight;

        /**
         * @brief Default constructor.
         *
         * This constructor is evaluated at compile time.
         */
        MU consteval LootItem() = default;

        DELETE_NEW_OPS;

        /**
         * @brief Determines if this loot item always produces exactly one item.
         *
         * @return @c true if @c MinRoll equals @c MaxRoll and both equal 1; otherwise, @c false.
         */
        static consteval bool isOnlyOneItem() noexcept { return MinRoll == MaxRoll && MinRoll == 1; }

        /**
         * @brief Determines if this loot item always produces a fixed number of items.
         *
         * @return @c true if @c MinRoll equals @c MaxRoll; otherwise, @c false.
         */
        static consteval bool isAlwaysNItems() noexcept { return MinRoll == MaxRoll; }

        /**
         * @brief Returns the weight of the loot item.
         *
         * @return The compile-time constant weight (<c>Weight</c>).
         */
        MU ND static consteval u16 weight() noexcept { return Weight; }

        /**
         * @brief Generates a @c lce::ItemState for this loot item.
         *
         * This function creates a @c lce::ItemState based on the default state and a random roll value.
         * If an optional function (@c Func) is provided, it is invoked to further modify the item state.
         *
         * @tparam ContainerSize The size of the container in which the item will be stored.
         * @tparam Legacy A flag to select between legacy and modern RNG behavior.
         * @param container A reference to the container where the generated item state is placed.
         * @param rng A reference to an RNG instance used to generate random values.
         * @return A @c lce::ItemState representing the generated loot item.
         *
         * @details
         * The function uses the static roll generator (@c myRoll) to determine the number of items to generate.
         * If <c>myFunc</c> is not @c nullptr, it is called to apply further modifications to the item state.
         */
        template<int ContainerSize, bool Legacy>
        MU lce::ItemState generateItemState(Container<ContainerSize> &container, RNG &rng) const {
            lce::ItemState itemStack(k_DefaultItemState);
            itemStack.setItemCount(k_Roll.template rollValue<Legacy>(rng));
            if constexpr (k_Func != nullptr) {
                k_Func(container, itemStack, rng);
            }
            return itemStack;
        }
    };

    /**
     * @brief Variant type for holding multiple @c LootItem types.
     *
     * This alias allows a single variable to store one of several different @c LootItem types.
     *
     * @tparam Ts One or more @c LootItem types.
     */
    template<typename... Ts>
    using LootItemVariant = std::variant<Ts...>;

} // namespace loot
