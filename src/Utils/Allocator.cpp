#include <Utils/Allocator.hpp>
#include <Utils/Log.hpp>

namespace Dynamo {
    Allocator::Allocator(unsigned capacity) : _capacity(capacity) {
        // Create the initial free block that encompasses the heap
        Block heap_block;
        heap_block.offset = 0;
        heap_block.size = capacity;
        _free.push_back(heap_block);
    }

    void Allocator::defragment(std::list<Block>::iterator it) {
#if defined(DYN_DEBUG) && defined(DYN_DEBUG_ALLOCATOR)
        Log::info("Defragmentation target: {} {}", it->offset, it->offset + it->size);
        Log::info("Before defragmentation: {}", print());
#endif
        Block block = *it;
        unsigned new_offset = block.offset;
        unsigned new_size = block.size;

        // Join left node
        if (it != _free.begin()) {
            auto left = std::prev(it);
            if (left->offset + left->size == block.offset) {
                new_offset = left->offset;
                new_size += left->size;
                _free.erase(left);
            }
        }

        // Join right node
        auto right = std::next(it);
        if (right != _free.end() && it->offset + it->size == right->offset) {
            new_size += right->size;
            _free.erase(right);
        }

        it->offset = new_offset;
        it->size = new_size;

#if defined(DYN_DEBUG) && defined(DYN_DEBUG_ALLOCATOR)
        Log::info("After defragmentation: {}", print());
#endif
    }

    std::optional<unsigned> Allocator::reserve(unsigned size, unsigned alignment) {
        DYN_ASSERT(size > 0);
        for (auto it = _free.begin(); it != _free.end(); it++) {
            Block &block = *it;

            unsigned offset = align_size(block.offset, alignment);
            unsigned block_r = block.offset + block.size;
            unsigned alloc_r = offset + size;

            if (block_r >= alloc_r) {
                // Handle allocation in the middle of the block due to alignment
                if (offset > block.offset) {
                    Block partition;
                    partition.offset = block.offset;
                    partition.size = offset - block.offset;
                    _free.insert(it, partition);
                }

                // Update the size and offset of the current block
                block.size = block_r - alloc_r;
                block.offset = alloc_r;

                // Block is empty, remove from freelist
                if (block.size == 0) {
                    _free.erase(it);
                }

                // Sanity check
                DYN_ASSERT(_used.count(offset) == 0);

                // Return the offset to the allocation and track it
                _used.emplace(offset, size);
                return offset;
            }
        }
        return {};
    }

    void Allocator::free(unsigned offset) {
        if (_used.count(offset) == 0) {
            Log::error("Allocator::free() failed, invalid offset {}: {}", offset, print());
        }

        // New free block
        Block freed;
        freed.offset = offset;
        freed.size = _used[offset];
        _used.erase(offset);

        // No free blocks
        if (_free.empty()) {
            _free.push_front(freed);
            return;
        }

        // Start of the list
        unsigned alloc_r = freed.offset + freed.size;
        if (_free.begin()->offset >= alloc_r) {
            _free.push_front(freed);
            defragment(_free.begin());
            return;
        }

        for (auto it = _free.begin(); it != _free.end(); it++) {
            unsigned block_r = it->offset + it->size;

            // End of the list
            auto next = std::next(it);
            if (next == _free.end()) {
                DYN_ASSERT(offset >= block_r);
                _free.push_back(freed);
                defragment(it);
                return;
            }

            // Between two blocks
            if (next->offset >= alloc_r && offset >= block_r) {
                defragment(_free.insert(next, freed));
                return;
            }
        }

        // Uh oh.
        DYN_ASSERT(false);
    }

    void Allocator::grow(unsigned capacity) {
#if defined(DYN_DEBUG) && defined(DYN_DEBUG_ALLOCATOR)
        Log::info("Before grow: {}", print());
#endif
        DYN_ASSERT(capacity >= _capacity);
        if (capacity == _capacity) {
            return;
        }
        Block new_free;
        new_free.offset = _capacity;
        new_free.size = capacity - _capacity;
        _capacity = capacity;

        // Add the new free block and defragment
        _free.push_back(new_free);
        defragment(std::prev(_free.end()));

#if defined(DYN_DEBUG) && defined(DYN_DEBUG_ALLOCATOR)
        Log::info("After grow: {}", print());
#endif
    }

    bool Allocator::is_reserved(unsigned offset) const { return _used.count(offset) > 0; }

    unsigned Allocator::capacity() const { return _capacity; }

    unsigned Allocator::size(unsigned offset) const { return _used.at(offset); }

    std::string Allocator::print() const {
        std::string str;
        if (_free.empty()) {
            return " | ******** | ";
        }

        // Allocation before the first free block
        auto it = _free.begin();
        if (it->offset > 0) {
            str += " | ********";
        }

        while (it != _free.end()) {
            unsigned right = it->offset + it->size;
            str += " | " + std::to_string(it->offset) + ", " + std::to_string(right);

            // Show the allocation before the next free block
            auto next = std::next(it);
            if (next != _free.end() && next->offset != right) {
                str += " | ********";
            }
            it++;
        }

        Block last = _free.back();
        if (last.offset + last.size != _capacity) {
            str += " | ********";
        }

        str += " | ";
        return str;
    }
} // namespace Dynamo