#include <Utils/Log.hpp>
#include <Utils/VirtualBuffer.hpp>

namespace Dynamo {
    VirtualBuffer::VirtualBuffer(unsigned capacity, unsigned alignment) :
        _buffer(capacity),
        _allocator(capacity),
        _alignment(alignment) {}

    unsigned VirtualBuffer::capacity() const { return _allocator.capacity(); }

    unsigned VirtualBuffer::size(unsigned block_offset) const { return _allocator.size(block_offset); }

    std::optional<unsigned> VirtualBuffer::reserve(unsigned size) { return _allocator.reserve(size, _alignment); }

    void VirtualBuffer::grow(unsigned capacity) {
        _allocator.grow(capacity);
        _buffer.resize(capacity);
    }

    void VirtualBuffer::free(unsigned block_offset) { _allocator.free(block_offset); }

    void *VirtualBuffer::mapped(unsigned block_offset) {
        DYN_ASSERT(_allocator.is_reserved(block_offset));
        return _buffer.data() + block_offset;
    }
} // namespace Dynamo