#pragma once

#include <cstdint>
#include <type_traits>
#include <vector>

#include <Utils/Log.hpp>

#define DYN_DEFINE_ID_TYPE(T) using T = struct T##_t *

namespace Dynamo {
    /**
     * @brief Data structure that allows O(1) insertion, removal, and access of a value by a unique handle identifier.
     *
     * Unlike SparseSets, Ids are tracked and assigned internally. This means that there is no risk of extreme
     * overallocation of entries.
     *
     * @tparam Id
     * @tparam T
     */
    template <typename Id, typename T>
    class SparseArray {
        static_assert(std::is_pointer<Id>::value, "Id must be a valid handle type (opaque pointer).");
        static_assert(sizeof(Id) == sizeof(uintptr_t), "Id and its integer mode must be the same size.");

        std::vector<T> _entries;
        std::vector<uint8_t> _set_flags;
        std::vector<uintptr_t> _recycle;
        unsigned _size = 0;

      public:
        /**
         * @brief Get the number of entries in the map.
         *
         * @return unsigned
         */
        unsigned size() const { return _size; }

        /**
         * @brief Check if the container is empty.
         *
         * @return true
         * @return false
         */
        bool empty() const { return _size == 0; }

        /**
         * @brief Insert a value into the set and return its id.
         *
         * @param value
         * @return Id
         */
        Id insert(const T &value) {
            _size++;
            if (_recycle.empty()) {
                _entries.push_back(value);
                _set_flags.push_back(1);
                return reinterpret_cast<Id>(_entries.size() - 1);
            } else {
                uintptr_t key = _recycle.back();
                _recycle.pop_back();

                _entries[key] = value;
                _set_flags[key] = 1;
                return reinterpret_cast<Id>(key);
            }
        }

        /**
         * @brief Remove an element from the set.
         *
         * @param id
         */
        void remove(Id id) {
            uintptr_t key = reinterpret_cast<uintptr_t>(id);
            DYN_ASSERT(key < _entries.size() && _set_flags[key]);
            _size--;
            _set_flags[key] = 0;
            _recycle.push_back(key);
        }

        /**
         * @brief Get an element from the set by id.
         *
         * @param id
         * @return T&
         */
        T &get(Id id) {
            uintptr_t key = reinterpret_cast<uintptr_t>(id);
            DYN_ASSERT(key < _entries.size() && _set_flags[key]);
            return _entries[key];
        }

        /**
         * @brief Get an element from the set by id.
         *
         * @param id
         * @return T&
         */
        const T &get(Id id) const {
            uintptr_t key = reinterpret_cast<uintptr_t>(id);
            DYN_ASSERT(key < _entries.size() && _set_flags[key]);
            return _entries[key];
        }

        /**
         * @brief Check if an entry exists in the array.
         *
         * @param id
         * @return true
         * @return false
         */
        bool exists(Id id) const {
            uintptr_t key = reinterpret_cast<uintptr_t>(id);
            return key < _entries.size() && _set_flags[key];
        }

        /**
         * @brief Iterate over each element in the set.
         *
         * @tparam Functor
         * @param function
         */
        template <typename Functor>
        void foreach (Functor &&function) {
            for (uintptr_t i = 0; i < _entries.size(); i++) {
                if (_set_flags[i]) {
                    function(_entries[i]);
                }
            }
        }

        /**
         * @brief Clear the map, invalidating all handles.
         *
         */
        void clear() {
            _size = 0;
            _entries.clear();
            _recycle.clear();
            _set_flags.clear();
        }
    };
} // namespace Dynamo