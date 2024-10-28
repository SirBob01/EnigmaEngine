#pragma once

#include <vector>

#include <Utils/Allocator.hpp>

namespace Dynamo {
    /**
     * @brief Virtual memory buffer.
     *
     */
    class VirtualMemory {
        std::vector<char> _buffer;
        Allocator _allocator;
        unsigned _alignment;

      public:
        VirtualMemory(unsigned initial_size, unsigned alignment = 1);
        VirtualMemory() = default;

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
         * @return unsigned
         */
        unsigned reserve(unsigned size);

        /**
         * @brief Free an allocated block.
         *
         * @param block_offset
         */
        void free(unsigned block_offset);

        /**
         * @brief Get the pointer to an allocated block.
         *
         * @return void*
         */
        void *get(unsigned block_offset);
    };
} // namespace Dynamo