#pragma once

#include "lce/items/Item.hpp"

namespace loot {

    /**
     * @class Buffer
     * @brief A simple utility class for storing and accessing two separate buffers of ItemState.
     *
     * This class provides two buffers (\c itemBuffer and \c stackBuffer), each capable of storing
     * a fixed number of \c ItemState objects. It is used in loot shuffling, allowing
     * temporary storage for items and stack-splitting operations.
     *
     * An initialized \c loot::Buffer should be passed to functions of the Loot class.
     */
    class Buffer {
        /// The fixed size of each internal buffer.
        static constexpr size_t BUFFER_SIZE = 128;

        /// Primary buffer used for storing generic \c lce::ItemState objects.
        lce::ItemState m_ItemBuffer[BUFFER_SIZE];

        /// Secondary buffer used for stackable \c lce::eItemState objects.
        lce::ItemState m_StackBuffer[BUFFER_SIZE];

    public:
        /**
         * @brief Constructs a Buffer with uninitialized memory for both buffers.
         */
        Buffer() : m_ItemBuffer(), m_StackBuffer() {}

        /**
         * @brief Provides direct access to the item buffer.
         * @return A pointer to the start of the primary item buffer.
         *
         * @note This pointer remains valid for the lifetime of the Buffer instance.
         */
        MU ND lce::ItemState* itemBuffer() noexcept {
            return &m_ItemBuffer[0];
        }

        /**
         * @brief Provides direct access to the stack buffer.
         * @return A pointer to the start of the secondary stack buffer.
         *
         * @note This pointer remains valid for the lifetime of the Buffer instance.
         */
        MU ND lce::ItemState* stackBuffer() noexcept {
            return &m_StackBuffer[0];
        }

        /**
         * @brief Retrieves the fixed size of each buffer.
         * @return The maximum number of ItemState objects storable in each buffer.
         */
        static size_t size() noexcept {
            return BUFFER_SIZE;
        }
    };

} // namespace loot
