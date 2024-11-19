#pragma once

#include <vector>

#include <Utils/Allocator.hpp>

namespace Dynamo {
    /**
     * @brief Virtual memory buffer.
     *
     */
    class VirtualBuffer {
        std::vector<char> _buffer;
        Allocator _allocator;
        unsigned _alignment;

      public:
        VirtualBuffer(unsigned capacity, unsigned alignment = 1);

        /**
         * @brief Get the capacity of the heap.
         *
         * @return unsigned
         */
        unsigned capacity() const;

        /**
         * @brief Get the size of a block.
         *
         * @param block_offset
         * @return unsigned
         */
        unsigned size(unsigned block_offset) const;

        /**
         * @brief Reserve a block.
         *
         * @param size
         * @return std::optional<unsigned>
         */
        std::optional<unsigned> reserve(unsigned size);

        /**
         * @brief Grow the heap.
         *
         * @param capacity
         */
        void grow(unsigned capacity);

        /**
         * @brief Free an allocated block.
         *
         * @param block_offset
         */
        void free(unsigned block_offset);

        /**
         * @brief Get the pointer to mapped memory.
         *
         * @return void*
         */
        void *mapped(unsigned block_offset);
    };
} // namespace Dynamo