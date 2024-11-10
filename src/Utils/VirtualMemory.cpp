#include <Utils/Log.hpp>
#include <Utils/VirtualMemory.hpp>

namespace Dynamo {
    VirtualMemory::VirtualMemory(unsigned capacity, unsigned alignment) :
        _buffer(capacity),
        _allocator(capacity),
        _alignment(alignment) {}

    unsigned VirtualMemory::capacity() const { return _allocator.capacity(); }

    unsigned VirtualMemory::size(unsigned block_offset) const { return _allocator.size(block_offset); }

    std::optional<unsigned> VirtualMemory::reserve(unsigned size) { return _allocator.reserve(size, _alignment); }

    void VirtualMemory::grow(unsigned capacity) {
        _allocator.grow(capacity);
        _buffer.resize(capacity);
    }

    void VirtualMemory::free(unsigned block_offset) { _allocator.free(block_offset); }

    void *VirtualMemory::mapped(unsigned block_offset) {
        DYN_ASSERT(_allocator.is_reserved(block_offset));
        return _buffer.data() + block_offset;
    }
} // namespace Dynamo