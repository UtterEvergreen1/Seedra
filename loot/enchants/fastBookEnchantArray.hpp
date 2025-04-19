#pragma once

/**
 * @file fastBookEnchantArray.hpp
 * @brief Provides helper classes for fast lookup of enchantment data for enchanted books.
 *
 * @details
 * This file defines helper structures used to build fast lookup arrays for enchantments when
 * processing enchanted books. It includes:
 *  - A templated index array (@c IndexArrayTemplate) that stores a fixed number of indices.
 *  - Two typedefs (@c IndexArraySmall and @c IndexArrayLarge) for different sizes of index arrays.
 *  - The @c EnchantLevelArray class, which aggregates enchantment state data and associated metadata
 *    such as total weight.
 *  - The @c EnchantLookupTable class, which provides a lookup table for enchanted book enchantments,
 *    supporting fast random selection based on cumulative weights.
 *
 * These types are designed for performance-critical code by leveraging fixed-size arrays and minimal
 * dynamic memory allocation.
 */

#include "common/rng.hpp"
#include "enchantState.hpp"
#include "lce/processor.hpp"

namespace enchants {

    /**
     * @brief Templated fixed-size index array.
     *
     * @tparam items The number of indices that can be stored.
     *
     * @details
     * The IndexArrayTemplate class is a simple fixed-size container for storing indices. It provides
     * basic operations such as adding an index, clearing the array, and retrieving stored values.
     * This is used by higher-level classes to manage lists of enchantment indices.
     */
    template<int items>
    class IndexArrayTemplate {
        static constexpr i8 ITEM_COUNT = items; ///< Maximum number of indices.
        i8 indexes[ITEM_COUNT] = {};              ///< Array storing indices.
        i8 currentIndex = 0;                      ///< Current count of indices stored.
    public:
        /**
         * @brief Default constructor.
         */
        IndexArrayTemplate() = default;

        /**
         * @brief Clears all stored indices.
         */
        void clear() { currentIndex = 0; }

        /**
         * @brief Retrieves the index value at a given position.
         *
         * @param indexIn The position from which to retrieve the index.
         * @return The stored index at the specified position.
         */
        ND int getValueAt(int indexIn) const { return indexes[indexIn]; }

        /**
         * @brief Retrieves the last stored index.
         *
         * @return The last index stored in the array.
         */
        ND int getLastValueIndex() const { return indexes[currentIndex - 1]; }

        /**
         * @brief Returns the current number of stored indices.
         *
         * @return The number of indices currently stored.
         */
        ND int getIndex() const { return currentIndex; }

        /**
         * @brief Adds a new index to the array.
         *
         * @param indexIn The index to add.
         */
        void addItem(c_int indexIn) { indexes[currentIndex++] = static_cast<i8>(indexIn); }

        /**
         * @brief Adjusts an input index based on the indices marked as "deleted".
         *
         * @param indexIn The original index.
         * @return The adjusted index after accounting for deletions.
         *
         * @details
         * This function iterates over the stored indices and decrements the input index if a stored
         * value is less than the current total count. It is used to map a logical index to the physical
         * index in the enchantment data array.
         */
        MU int getEnchantIndex(int indexIn) {
            for (int i = 0; i < currentIndex; i++)
                if (currentIndex > indexes[i])
                    indexIn--;
            return indexIn;
        }
    };

    /// Type alias for a small index array (capacity of 12 indices).
    typedef IndexArrayTemplate<12> IndexArraySmall;

    /// Type alias for a larger index array (capacity of 16 indices).
    typedef IndexArrayTemplate<16> IndexArrayLarge;


    /**
     * @brief Aggregates enchantment state data for a given level.
     *
     * @details
     * The EnchantLevelArray class stores an array of EnchantState objects, along with metadata
     * such as the total weight of the enchantments and the number of enchantments stored.
     * It also contains two index arrays: one for marking deleted entries and one for active indices.
     * This structure is used by the lookup table for quickly selecting enchantments for enchanted books.
     */
    class EnchantLevelArray {
    public:
        i8 totalWeight = 0;   ///< Total weight of all stored enchantments.
        i8 totalEnchants = 0; ///< Total count of enchantments stored.

        IndexArrayLarge deletions = IndexArrayLarge(); ///< Indexes of "deleted" enchantments.
        IndexArraySmall enchants = IndexArraySmall();    ///< Active indexes for enchantments.
        EnchantState data[MAX_ENCHANTMENT_COUNT] = {};   ///< Array of enchantment states.

        /**
         * @brief Default constructor.
         */
        EnchantLevelArray() = default;

        /**
         * @brief Adds an enchantment state to the array.
         *
         * @param ench A pointer to the Enchant object.
         * @param level The level at which the enchantment is applied.
         *
         * @details The function constructs an EnchantState using the enchantment's ID and the provided level,
         * and then appends it to the data array.
         */
        void addData(const Enchant* ench, int level);

        /**
         * @brief Clears the index arrays for deletions and active enchantments.
         */
        void clear() {
            deletions.clear();
            enchants.clear();
        }

        /**
         * @brief Retrieves an enchantment state by index.
         *
         * @param indexIn The logical index (after accounting for deletions).
         * @return The EnchantState at the specified index.
         */
        MU EnchantState getIndex(int indexIn);

        /**
         * @brief Retrieves the last enchantment state added.
         *
         * @return The last EnchantState in the active list.
         */
        EnchantState getLastEnchantAdded();

        /**
         * @brief Randomly selects and adds an enchantment index to the active list.
         *
         * @param rng A reference to an RNG instance used for selection.
         *
         * @details This function uses the total weight and the active deletion list to randomly select
         * an enchantment from the stored data, and then adds its index to the active list.
         */
        void addRandomItem(RNG& rng);
    };


    /**
     * @brief Lookup table for enchantments (used for enchanted books).
     *
     * @details
     * The EnchantLookupTable class provides a way to quickly access EnchantLevelArray instances
     * based on a cost value. It maintains an array of pointers to EnchantLevelArray objects,
     * along with flags indicating if the table has been set up.
     *
     * @note This table is currently used only for enchanted books.
     */
    class EnchantLookupTable {
        static constexpr i8 VECTOR_COUNT = 48; ///< Number of cost buckets available.
        EnchantLevelArray* dataArrays[VECTOR_COUNT] = {nullptr}; ///< Array of pointers to EnchantLevelArray objects.
        bool myIsSetup = false; ///< Flag indicating whether the lookup table has been set up.
    public:
        /// Total weight of all enchantments (computed during setup).
        static i8 TOTAL_WEIGHT;

        /// Cumulative weight array for fast lookup (indexed by enchantment count).
        static i8 CUMULATIVE_WEIGHT_ALL[MAX_ENCHANTMENT_COUNT];

        /**
         * @brief Default constructor.
         */
        EnchantLookupTable() = default;

        /**
         * @brief Constructs an EnchantLookupTable and optionally allocates resources.
         *
         * @param allocate If @c true, automatically calls setup().
         */
        MU explicit EnchantLookupTable(c_bool allocate) { if (allocate) { setup(); } }

        /**
         * @brief Destructor that deallocates any allocated resources.
         */
        ~EnchantLookupTable() { deallocate(); }

        /**
         * @brief Checks if the lookup table has been set up.
         *
         * @return @c true if the table is set up; otherwise, @c false.
         */
        ND MU bool isSetup() const { return myIsSetup; }

        /**
         * @brief Retrieves the EnchantLevelArray for a given cost.
         *
         * @param cost The enchantment cost level.
         * @return A pointer to the corresponding EnchantLevelArray.
         *
         * @details The function clears the array before returning it to ensure no stale data remains.
         */
        ND EnchantLevelArray* get(int cost) const;

        /**
         * @brief Sets up the lookup table.
         *
         * @details
         * This function initializes the lookup table by iterating over active enchantments,
         * computing cumulative weights, and populating the cost buckets.
         */
        void setup();

    private:
        /**
         * @brief Deallocates all EnchantLevelArray objects and resets the lookup table.
         */
        void deallocate();
    };

} // namespace enchants
