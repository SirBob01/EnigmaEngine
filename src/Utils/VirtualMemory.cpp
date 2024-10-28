#include <Utils/Log.hpp>
#include <Utils/VirtualMemory.hpp>

namespace Dynamo {
    VirtualMemory::VirtualMemory(unsigned initial_size, unsigned alignment) :
        _buffer(initial_size), _allocator(initial_size), _alignment(alignment) {}

    unsigned VirtualMemory::size(unsigned block_offset) const { return _allocator.size(block_offset); }

    unsigned VirtualMemory::reserve(unsigned size) {
        std::optional<unsigned> result = _allocator.reserve(size, _alignment);
        if (result.has_value()) {
            return result.value();
        } else {
            unsigned curr = _allocator.capacity();
            unsigned next = std::max(curr + size, curr * 2);
            unsigned next_aligned = align_size(next, _alignment);
            _allocator.grow(next_aligned);
            _buffer.resize(next_aligned);

            // If this fails, we're in trouble...
            return _allocator.reserve(size, _alignment).value();
        }
    }

    void VirtualMemory::free(unsigned block_offset) { _allocator.free(block_offset); }

    void *VirtualMemory::get_mapped(unsigned block_offset) {
        DYN_ASSERT(_allocator.is_reserved(block_offset));
        return _buffer.data() + block_offset;
    }
} // namespace Dynamo