#include "fastBookEnchantArray.hpp"
#include "enchantController.hpp"

/**
 * @file fastBookEnchantArray.cpp
 * @brief Implements functions for fast lookup of enchantment data used in enchanted books.
 *
 * @details
 * This file implements methods for managing an EnchantLevelArray that holds the state of enchantments
 * along with cumulative weight information. It also defines functions for updating the lookup table
 * (EnchantLookupTable) by computing cumulative weights and populating cost buckets.
 */

namespace enchants {

    /**
     * @brief Adds an enchantment state to the array.
     *
     * @param ench Pointer to the Enchant object.
     * @param level The level at which the enchantment is applied.
     *
     * @details
     * The function creates an EnchantState from the given Enchant object (using its ID) and level,
     * and appends it to the @c data array. The total number of enchantments is incremented.
     */
    void EnchantLevelArray::addData(const Enchant* ench, const int level) {
        data[totalEnchants++] = EnchantState(ench->getID(), level, 0);
    }

    /**
     * @brief Retrieves an EnchantState from the array based on a logical index.
     *
     * @param indexIn The original index, which is adjusted for deletions.
     * @return The EnchantState at the adjusted index.
     *
     * @details
     * The provided index is modified by calling @c deletions.getEnchantIndex() to account for any deleted
     * entries, then the corresponding EnchantState is returned.
     */
    EnchantState EnchantLevelArray::getIndex(int indexIn) {
        indexIn = deletions.getEnchantIndex(indexIn);
        return data[indexIn];
    }

    /**
     * @brief Retrieves the last EnchantState that was added to the active list.
     *
     * @return The last EnchantState from the active enchant indices.
     */
    EnchantState EnchantLevelArray::getLastEnchantAdded() {
        return data[enchants.getLastValueIndex()];
    }

    /**
     * @brief Randomly selects an enchantment index to add to the active list.
     *
     * @param rng A reference to an RNG instance used for random selection.
     *
     * @details
     * The function first computes the total effective weight by subtracting the weight of any deleted entries.
     * Then, it generates a random weight and uses either a fast binary search (if the total weight equals the
     * expected full total) or a linear search to find the corresponding enchantment. Once found, the index is
     * added to the active list.
     */
    void EnchantLevelArray::addRandomItem(RNG& rng) {
        // Calculate the effective total weight by subtracting weights for deleted entries.
        int theTotalWeight = static_cast<u8>(this->totalWeight);
        for (int i = 0; i < deletions.getIndex(); i++) {
            c_int enchIndex = deletions.getValueAt(i);
            theTotalWeight -= EnchantController::getWeightFromID(data[enchIndex].getID());
        }

        // Generate a random weight.
        int weight = rng.nextInt(theTotalWeight);

        // If the effective weight equals the full total, perform a fast binary search.
        if (theTotalWeight == EnchantLookupTable::TOTAL_WEIGHT) {
            size_t low = 0;
            size_t high = EnchantController::getActiveSize();
            while (low < high) {
                // Compute the middle index.
                if (const size_t mid = (low + high) >> 1; EnchantLookupTable::CUMULATIVE_WEIGHT_ALL[mid] > weight) {
                    high = mid;
                } else {
                    low = mid + 1;
                }
            }
            enchants.addItem(static_cast<int>(low));
            return;
        }

        // Otherwise, use a linear search over the enchantment states.
        for (int enchIndex = 0; enchIndex < totalEnchants; enchIndex++) {
            bool isDeleted = false;
            for (int i = 0; i < deletions.getIndex(); i++) {
                if (enchIndex == deletions.getValueAt(i)) {
                    isDeleted = true;
                    break;
                }
            }
            if (isDeleted) {
                continue;
            }
            weight -= EnchantController::getWeightFromID(data[enchIndex].getID());
            // When weight falls below zero, the corresponding index is selected.
            if (weight < 0) {
                enchants.addItem(enchIndex);
                return;
            }
        }
    }

    //==============================================================================
    //                    EnchantedBookEnchantsLookupTable
    //==============================================================================

    /**
     * @brief Retrieves the EnchantLevelArray for a specified cost.
     *
     * @param cost The enchantment cost level.
     * @return A pointer to the corresponding EnchantLevelArray after clearing it.
     *
     * @details
     * The function accesses the cost bucket in the lookup table and clears its data before returning it.
     * It is assumed that the cost is within a valid range.
     *
     * @warning The cost must be between 0 and the maximum vector count (VECTOR_COUNT); otherwise, behavior is undefined.
     */
    EnchantLevelArray* EnchantLookupTable::get(c_int cost) const {
        EnchantLevelArray* array = dataArrays[cost];
        array->clear();
        return array;
    }

    /**
     * @brief Sets up the lookup table for enchanted book enchantments.
     *
     * @details
     * This function initializes the lookup table by deallocating any previous data, then iterates over
     * the active enchantments to compute a global cumulative weight. It then allocates an EnchantLevelArray for
     * each cost bucket and populates it with valid enchantments (those whose cost falls within the valid range).
     * If any active enchant pointer is null, a std::runtime_error is thrown.
     */
    void EnchantLookupTable::setup() {
        deallocate();

        const auto activeEnchants = EnchantController::getActiveEnchants();
        const auto activeSize = EnchantController::getActiveSize();

        // Compute global cumulative weights.
        int sum = 0;
        for (size_t i = 0; i < activeSize; ++i) {
            const Enchant* ench = activeEnchants[i];
            sum += ench->getRarity();
            CUMULATIVE_WEIGHT_ALL[i] = static_cast<i8>(sum);
        }
        TOTAL_WEIGHT = static_cast<i8>(sum);

        // Allocate and populate cost buckets.
        for (int cost = 0; cost < VECTOR_COUNT; cost++) {
            dataArrays[cost] = new EnchantLevelArray();
            c_auto array = dataArrays[cost];

            MU int i = 0;
            for (const Enchant* ench_pt: activeEnchants) {
                if EXPECT_FALSE(ench_pt == nullptr) {
                    throw std::runtime_error("Enchant pointer is NULL; the table is incorrect!");
                }
                for (int level = ench_pt->getMaxLevel(); level > 0; --level) {
                    if (ench_pt->isValidCost(level, cost)) {
                        array->addData(ench_pt, level);
                        array->totalWeight += ench_pt->getRarity();
                        break;
                    }
                }
                i++;
            }
        }
        myIsSetup = true;
    }

    /**
     * @brief Deallocates all EnchantLevelArray objects in the lookup table.
     *
     * @details This function iterates over the internal array and deletes each allocated EnchantLevelArray,
     * then resets the pointers and marks the table as not set up.
     */
    void EnchantLookupTable::deallocate() {
        for (auto& vector: dataArrays) {
            delete vector;
            vector = nullptr;
        }
        myIsSetup = false;
    }

    // Define static members.
    i8 EnchantLookupTable::TOTAL_WEIGHT = 0;
    i8 EnchantLookupTable::CUMULATIVE_WEIGHT_ALL[MAX_ENCHANTMENT_COUNT] = {};
}
