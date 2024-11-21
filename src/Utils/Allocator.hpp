#pragma once

#include <list>
#include <optional>
#include <string>
#include <unordered_map>

// Enable this to visualize heap growth and defragmentation behavior
// #define DEBUG_ALLOCATOR

namespace Dynamo {
    /**
     * @brief Round up a size to be a multiple of alignment.
     *
     * @param size      Size in bytes.
     * @param alignment Alignment in bytes.
     * @return unsigned
     */
    inline unsigned align_size(unsigned size, unsigned alignment) {
        return ((size + alignment - 1) / alignment) * alignment;
    }

    /**
     * @brief Implements the allocation and deallocation strategy for
     * dynamic virtual heap memory management.
     *
     */
    class Allocator {
        struct Block {
            unsigned offset;
            unsigned size;
        };
        std::list<Block> _free;
        std::unordered_map<unsigned, unsigned> _used;

        unsigned _capacity;

        /**
         * @brief Join adjacent blocks.
         *
         * @param it Reference block.
         */
        void defragment(std::list<Block>::iterator it);

      public:
        /**
         * @brief Construct a new Allocator object.
         *
         * @param capacity Capacity of the heap.
         */
        Allocator(unsigned capacity = 0);

        /**
         * @brief Reserve a block of memory with specific alignment
         * requirements, returning the offset within the pool.
         *
         * @param size      Desired size in bytes.
         * @param alignment Alignment requirement in bytes.
         * @return std::optional<unsigned>
         */
        std::optional<unsigned> reserve(unsigned size, unsigned alignment);

        /**
         * @brief Free the block of reserved memory at an offset.
         *
         * @param offset Offset within the pool in bytes returned by reserve().
         */
        void free(unsigned offset);

        /**
         * @brief Grow the total capacity, expanding the free blocks.
         *
         * @param capacity New capacity in bytes >= current_capacity.
         */
        void grow(unsigned capacity);

        /**
         * @brief Check if an offset is mapped to a reserved block.
         *
         * @param offset Offset within the pool in bytes returned by reserve().
         * @return true
         * @return false
         */
        bool is_reserved(unsigned offset) const;

        /**
         * @brief Get the capacity of the allocator.
         *
         * @return unsigned
         */
        unsigned capacity() const;

        /**
         * @brief Get the size of a reserved block.
         *
         * @param offset Offset within the pool in bytes returned by reserve().
         * @return unsigned
         */
        unsigned size(unsigned offset) const;

        /**
         * @brief Generate the human-readable string to visualize the state
         * of the allocator (for debugging).
         *
         * @return std::string
         */
        std::string print() const;
    };
} // namespace Dynamo