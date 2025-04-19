#pragma once

/**
 * @file Container.hpp
 * @brief Declares the Container class for storing loot items and enchantments.
 *
 * @details
 * The Container class template is a fixed-size container used to store
 * @c lce::ItemState objects representing loot items, and their associated enchantments
 * (via @c enchants::EnchantState). It provides functions for adding items and enchantments,
 * retrieving individual elements, clearing the container, and shuffling the stored items.
 * The class uses a custom fixed-vector type and includes helper functions for shuffling
 * arrays of items.
 */

#include <array>
#include <cstring>
#include <map>
#include <numeric>
#include "common/fixedVector.hpp"
#include "common/rng.hpp"
#include "lce/registry/itemRegistry.hpp"
#include "loot/enchants/enchantController.hpp"
#include "loot/enchants/enchantState.hpp"

namespace loot {

    /**
     * @brief Fixed-size container for loot items and enchantments.
     *
     * The @c Container class template stores a fixed number of @c lce::ItemState objects
     * in its slots and a fixed number of @c enchants::EnchantState objects for enchantments.
     * It supports adding, accessing, and shuffling items and enchantments.
     *
     * @tparam N The maximum number of loot items (slots).
     * @tparam E The maximum number of enchantments (default is 64).
     */
    template<size_t N, size_t E = 64>
    class MU Container final {
        /// Fixed vector holding the loot item states.
        FixedVector<lce::ItemState, N> m_Slots;

        /// Fixed vector holding the enchantment states.
        FixedVector<enchants::EnchantState, E> m_Enchants;

    public:
        /**
     * @brief Constructs an empty container.
     */
        Container() : m_Slots(), m_Enchants() {}

        /**
         * @brief Returns the current number of stored loot items.
         *
         * @return The number of items in the container.
         */
        MU ND size_t slotCount() const noexcept { return m_Slots.current_size(); }

        /**
         * @brief Retrieves the loot item state at the specified index.
         *
         * @param index The index of the slot.
         * @return The @c lce::ItemState stored at the given index.
         *
         * @note The function does not perform bounds checking.
         */
        MU ND lce::ItemState getSlotAt(const i32 index) const { return m_Slots[index]; }

        /**
         * @brief Sets the loot item state at the specified index.
         *
         * @tparam updateContainerIndex A flag indicating whether to update the container index
         *                              in the item state.
         * @param index The index at which to set the item.
         * @param itemState The item state to set.
         */
        template<bool updateContainerIndex = true>
        MU void setSlot(const i32 index, const lce::ItemState &itemState) {
            m_Slots[index] = itemState;
            if constexpr (updateContainerIndex) {
                m_Slots[index].myContainerIndex = m_Slots.current_size() - 1;
            }
        }

        /**
         * @brief Adds a loot item state to the container.
         *
         * @param itemState The loot item state to add.
         */
        void addItem(lce::ItemState itemState) {
            m_Slots.push_back(itemState);
            m_Slots[m_Slots.current_size() - 1].setContainerIndex(m_Slots.current_size() - 1);
        }

        /**
         * @brief Returns the current number of stored enchantments.
         *
         * @return The number of enchantments in the container.
         */
        MU ND size_t enchantCount() const noexcept { return m_Enchants.current_size(); }

        /**
         * @brief Retrieves the enchantment state at the specified index.
         *
         * @param index The index of the enchantment.
         * @return The @c enchants::EnchantState stored at the given index.
         *
         * @note The function does not perform bounds checking.
         */
        MU ND enchants::EnchantState getEnchantAt(const i32 index) const { return m_Enchants[index]; }

        /**
         * @brief Adds an enchantment state to the container.
         *
         * @param enchantData The enchantment state to add.
         */
        void addEnchant(const enchants::EnchantState enchantData) {
            m_Enchants.push_back(enchantData);
            m_Enchants[m_Enchants.current_size() - 1].setExtra(m_Slots.current_size());
        }

        /**
     * @brief Clears all items and enchantments from the container.
     */
        void clear() {
            m_Slots.set_size(0);
            m_Enchants.set_size(0);
        }

        /**
         * @brief Prints the combined items and their enchantments.
         *
         * This function aggregates similar items and prints their combined counts
         * along with their associated enchantments.
         */
        MU void printCombinedItems() const;

    private:
        /**
         * @brief Overloaded stream insertion operator for the container.
         *
         * @param out The output stream.
         * @param container The container to output.
         * @return A reference to the output stream.
         *
         * @details
         * The operator builds a string representation of the container by iterating over
         * the stored loot items and formatting their details along with any associated
         * enchantments.
         */
        friend std::ostream &operator<<(std::ostream &out, const Container &container) {
            std::string buffer;
            buffer.reserve(1024);

            std::map<int, std::vector<enchants::EnchantState>> itemIndexToEnchantStateMap;
            const size_t enchantCount = container.enchantCount();
            for (size_t enchantIndex = 0; enchantIndex < enchantCount; enchantIndex++) {
                const enchants::EnchantState &enchState = container.getEnchantAt(enchantIndex);
                const enchants::Enchant* ench = enchants::EnchantController::getEnchantFromID(enchState.getID());
                if (ench != nullptr) {
                    itemIndexToEnchantStateMap[enchState.getExtra()].push_back(enchState);
                }
            }

            buffer += "\n{";

            const size_t containerIndex = container.slotCount();
            for (size_t i = 0; i < containerIndex; i++) {
                const lce::ItemState &itemState = container.getSlotAt(i);
                if (itemState.getCount() == 0) {
                    continue;
                }

                buffer += "\n";
                buffer += std::to_string(i);
                buffer += ": ";

                lce::Item const *item = lce::registry::ItemRegistry::getItem(
                        itemState.getID(), itemState.getDataTag());
                buffer += item->getName();

                if (itemState.getCount() > 1) {
                    buffer += " (";
                    buffer += std::to_string(itemState.getCount());
                    buffer += ")";
                }

                auto it = itemIndexToEnchantStateMap.find(itemState.getContainerIndex());
                if (it != itemIndexToEnchantStateMap.end()) {
                    buffer += " [";
                    for (size_t j = 0; j < it->second.size(); ++j) {
                        buffer += it->second[j].toString();
                        if (j < it->second.size() - 1) {
                            buffer += ", ";
                        }
                    }
                    buffer += "]";
                }
            }

            buffer += "\n}\n";

            out << buffer;
            return out;
        }

        /**
         * @brief Shuffles an array of indices using the provided RNG.
         *
         * @param arr The array of indices to shuffle.
         * @param rng A reference to an RNG instance.
         *
         * @details
         * The function implements a Fisher-Yates shuffle algorithm to randomly reorder the elements
         * in @c arr.
         */
        static void shuffleIndices(std::array<int, N> &arr, RNG &rng) noexcept {
            for (int rangeLimit = N - 1; rangeLimit > 0; --rangeLimit) {
                int randomIndex = rng.nextInt(rangeLimit);
                if (rangeLimit != randomIndex) {
                    std::swap(arr[rangeLimit], arr[randomIndex]);
                }
            }
        }

        /**
         * @brief Shuffles a raw array of data in place.
         *
         * @tparam T The type of the array elements.
         * @param data Pointer to the array to shuffle.
         * @param count The number of elements in the array.
         * @param rng A reference to an RNG instance.
         *
         * @details
         * This template function implements a shuffle algorithm similar to Fisher-Yates, swapping
         * elements randomly within the array.
         */
        template<typename T>
        static void shuffleArray(T *data, const int count, RNG &rng) {
            for (int rangeLimit = count - 1; rangeLimit > 0; --rangeLimit) {
                int randomIndex = rng.nextInt(rangeLimit);
                if (rangeLimit != randomIndex) {
                    std::swap(data[rangeLimit], data[randomIndex]);
                }
            }
        }

    public:

        /**
         * @brief Shuffles the container's items using a temporary buffer.
         *
         * @param rng A reference to an RNG instance for randomization.
         * @param buffer A pointer to a @c Buffer used for temporary storage during shuffling.
         *
         * @details
         * This function copies the current items into a temporary buffer, resets the container,
         * shuffles the order of the items, and then places them back into the container.
         * It handles splitting stackable items if necessary.
         */
        void shuffleInto(RNG rng, Buffer *buffer) {
            int theCount = 0;

            lce::ItemState *itemBuffer = buffer->itemBuffer();
            lce::ItemState *stackBuffer = buffer->stackBuffer();

            const int currentSize = (int) m_Slots.current_size();
            memcpy(itemBuffer, m_Slots.data(), currentSize * sizeof(lce::ItemState));
            theCount = currentSize;

            memset(m_Slots.data(), 0, N * sizeof(lce::ItemState));
            m_Slots.set_size(N);

            std::array<int, N> slotOrder{};
            std::iota(slotOrder.begin(), slotOrder.end(), 0);
            shuffleIndices(slotOrder, rng);

            int slotOrderIndex = N - 1;
            int stackableCount = 0;

            for (int i = 0; i < theCount; i++) {
                if (itemBuffer[i].getCount() > 1) {
                    stackBuffer[stackableCount++] = itemBuffer[i];
                }
            }

            int writeIndex = 0;
            for (int i = 0; i < theCount; i++) {
                if (itemBuffer[i].getCount() <= 1) {
                    itemBuffer[writeIndex++] = itemBuffer[i];
                }
            }
            theCount = writeIndex;

            int numSlots = N;
            numSlots -= theCount;

            while (numSlots > 0 && stackableCount > 0) {
                const int itemIndex = rng.nextInt(0, stackableCount - 1);
                lce::ItemState originalStack = stackBuffer[itemIndex];

                for (int j = itemIndex; j < stackableCount - 1; j++) {
                    stackBuffer[j] = stackBuffer[j + 1];
                }
                stackableCount--;

                const int splitAmount = rng.nextInt(1, (originalStack.getCount() >> 1));
                lce::ItemState splittedStack = originalStack.split(splitAmount);

                if (originalStack.getCount() == 0 || rng.next<1>() == 0) {
                    itemBuffer[theCount++] = originalStack;
                } else {
                    stackBuffer[stackableCount++] = originalStack;
                }

                if (splittedStack.getCount() == 0 || rng.next<1>() == 0) {
                    itemBuffer[theCount++] = splittedStack;
                } else {
                    stackBuffer[stackableCount++] = splittedStack;
                }
            }

            if (stackableCount > 0) {
                memcpy(&itemBuffer[theCount], stackBuffer, stackableCount * sizeof(lce::ItemState));
                theCount += stackableCount;
            }

            shuffleArray(itemBuffer, theCount, rng);

            for (int i = 0; i < theCount; i++) {
                if (slotOrderIndex < 0) {
                    // Tried to overfill the container.
                    return;
                }
                if (itemBuffer[i].getCount() > 0) {
                    const int placeIndex = slotOrder[slotOrderIndex--];
                    setSlot<false>(placeIndex, itemBuffer[i]);
                } else {
                    --slotOrderIndex;
                }
            }
        }
    };

    /// Prints a combined view of the container's items.
    template<size_t N, size_t E>
    MU void Container<N, E>::printCombinedItems() const {
        std::map<lce::ItemState, int> itemCount;
        for (const auto &itemState: m_Slots) {
            if (itemState.getCount() > 0) {
                if (itemCount.contains(itemState)) {
                    itemCount[itemState] += itemState.getCount();
                } else {
                    itemCount[itemState] = itemState.getCount();
                }
            }
        }
        // For debugging:
        // for (const auto& [fst, snd]: itemCount) {
        //     const auto ItemState = ItemState(fst, snd);
        //     std::cout << ItemState << std::endl;
        // }
    }

    /// Explicit template instantiations.
    template class Container<27, 64>;
    template class Container<9, 64>;

} // namespace loot
