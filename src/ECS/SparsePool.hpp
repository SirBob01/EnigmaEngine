#pragma once

#include <vector>

#include <Utils/SparseArray.hpp>

namespace Dynamo::ECS {
    /**
     * @brief Entity handle.
     *
     */
    DYN_DEFINE_ID_TYPE(Entity);

    /**
     * @brief Sparse set of components.
     *
     */
    class SparsePool {
        static constexpr uintptr_t NULL_INDEX = static_cast<uintptr_t>(-1);
        unsigned N = 0;

        // Byte buffer of components
        std::vector<unsigned char> _buffer;

        // Indices to the pool
        std::vector<uintptr_t> _sparse;

        // Entity members, indexed by the sparse array
        std::vector<Entity> _dense;

      public:
        void initialize(unsigned size) { N = size; }

        bool invalid() { return N == 0; }

        unsigned size() const { return _dense.size(); }

        template <typename Component>
        void insert(Entity entity, Component &&component) {
            DYN_ASSERT(!exists(entity));
            uintptr_t key = reinterpret_cast<uintptr_t>(entity);
            if (key >= _sparse.size()) {
                _sparse.resize((key + 1) * 2, NULL_INDEX);
            }

            // Update sparse and dense arrays
            _sparse[key] = _dense.size();
            _dense.push_back(entity);

            // Write the component to the end of the buffer
            unsigned offset = _buffer.size();
            _buffer.resize(offset + sizeof(Component));
            std::memcpy(_buffer.data() + offset, &component, sizeof(Component));
        }

        bool exists(Entity &entity) const {
            uintptr_t key = reinterpret_cast<uintptr_t>(entity);
            return key < _sparse.size() && _sparse[key] != NULL_INDEX;
        }

        void remove(Entity entity) {
            DYN_ASSERT(exists(entity));
            uintptr_t key = reinterpret_cast<uintptr_t>(entity);
            uintptr_t index = _sparse[key];

            // Swap last element of dense arrays to maintain contiguity
            Entity back_entity = _dense.back();
            uintptr_t back_key = reinterpret_cast<uintptr_t>(back_entity);

            // Update buffer
            std::memcpy(_buffer.data() + index * N, _buffer.data() + _buffer.size() - N, N);
            _buffer.resize(_buffer.size() - N);

            // Update dense array
            _dense[index] = back_entity;
            _dense.pop_back();

            // Update sparse set, pointing to newly swapped object
            _sparse[back_key] = index;
            _sparse[key] = NULL_INDEX;
        }

        const std::vector<Entity> &dense() const { return _dense; }

        template <typename Component>
        Component &get(unsigned index) {
            unsigned offset = index * sizeof(Component);
            DYN_ASSERT(offset < _buffer.size());
            return *reinterpret_cast<Component *>(_buffer.data() + offset);
        }

        template <typename Component>
        Component &get(Entity entity) {
            uintptr_t key = reinterpret_cast<uintptr_t>(entity);
            DYN_ASSERT(key < _sparse.size());
            return get<Component>(_sparse[key]);
        }

        template <typename Component, typename Functor>
        void foreach (Functor function) {
            for (unsigned i = 0; i < _dense.size(); i++) {
                unsigned offset = i * sizeof(Component);
                function(_dense[i], *reinterpret_cast<Component *>(_buffer.data() + offset));
            }
        }

        void clear() {
            _buffer.clear();
            _sparse.clear();
            _dense.clear();
        }
    };
} // namespace Dynamo::ECS