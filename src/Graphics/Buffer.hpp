#pragma once

#include <Utils/SparseArray.hpp>

namespace Dynamo::Graphics {
    /**
     * @brief Buffer resource handle.
     *
     */
    DYN_DEFINE_ID_TYPE(Buffer);

    /**
     * @brief Buffer usage modes.
     *
     */
    enum class BufferUsage : uint8_t {
        Vertex,
        Index,
        Uniform,
        Storage,
        Staging,
    };

    /**
     * @brief Buffer memory properties.
     *
     */
    enum class MemoryProperty : uint8_t {
        HostVisible,
        DeviceLocal,
    };

    /**
     * @brief Buffer object
     *
     */
    struct BufferDescriptor {
        /**
         * @brief Size of the buffer in bytes.
         *
         */
        unsigned size;

        /**
         * @brief Buffer usage mode.
         *
         */
        BufferUsage usage;

        /**
         * @brief Buffer memory property.
         *
         */
        MemoryProperty property;
    };
} // namespace Dynamo::Graphics