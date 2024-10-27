#pragma once

#include <algorithm>
#include <vector>

#include <Utils/Log.hpp>

namespace Dynamo {
    /**
     * @brief Sparse sets are an alternative to the hash map that allow
     * for efficient association of data with unique identifier keys. Both keys
     * and values are stored in contiguous memory for improved cache locality,
     * mapping one-to-one.
     *
     * This means that the complexity of hot operations can be reduced to
     * constant time at the cost of space.
     *
     * Insertion: O(1) ammortized.
     * Deletion: O(1).
     * Search: O(1).
     *
     * @tparam Id Unique handle type
     * @tparam T  Type of element
     */
    template <typename Id, typename T>
    class SparseSet {
        /**
         * @brief Contains the actual item data
         *
         */
        std::vector<T> _pool;

        /**
         * @brief Contains unique identifiers that map one-to-one with _pool
         *
         */
        std::vector<Id> _dense;

        /**
         * @brief Contains indices to _dense and _pool
         *
         */
        std::vector<uintptr_t> _sparse;

      public:
        /**
         * @brief Get the number of items.
         *
         * @return unsigned
         */
        inline unsigned size() const { return _pool.size(); }

        /**
         * @brief Check if the container is empty.
         *
         * @return true
         * @return false
         */
        inline bool empty() const { return size() == 0; }

        /**
         * @brief Find the position of a value within the dense array.
         *
         * @param id Unique identifer.
         * @return int Index position of the value (-1 on failure).
         */
        inline int find(Id id) const {
            uintptr_t key = reinterpret_cast<uintptr_t>(id);
            if (key >= _sparse.size()) {
                return -1;
            }

            // Verify that the sparse and dense arrays are correlated
            uintptr_t index = _sparse[key];
            uintptr_t dense_size = _dense.size();
            if (index == -1 || index >= dense_size || _dense[index] != id) {
                return -1;
            }
            return index;
        }

        /**
         * @brief Test if an id exists within the set.
         *
         * @param id
         * @return true
         * @return false
         */
        inline bool exists(Id id) const { return find(id) >= 0; }

        /**
         * @brief Create a new object in the pool and associate it with an id.
         *
         * If the id's index exists in the set, it is overwritten.
         *
         * @tparam Params Member types of the composite data type.
         * @param id   Unique identifer of the constructed object.
         * @param args Parameter values used to construct the object.
         */
        template <typename... Params>
        inline void insert(Id id, Params... args) {
            // Resize the sparse array or remove the existing item
            uintptr_t key = reinterpret_cast<uintptr_t>(id);
            if (key >= _sparse.size()) {
                _sparse.resize((key + 1) * 2, -1);
            } else if (_sparse[key] != -1) {
                remove(_dense[_sparse[key]]);
            }

            // Update dense and pool arrays
            _dense.push_back(id);
            if constexpr (std::is_aggregate<T>::value) {
                _pool.push_back(T{args...});
            } else {
                _pool.emplace_back(args...);
            }

            // Update sparse array to point to the last element
            _sparse[key] = _dense.size() - 1;
        }

        /**
         * @brief Remove an item from the set, doing nothing if it does not
         * exist.
         *
         * @param id Unique identifier of the object to be removed.
         */
        inline void remove(Id id) {
            uintptr_t key = reinterpret_cast<uintptr_t>(id);
            if (key >= _sparse.size()) {
                return;
            }

            // Verify that the sparse and dense arrays are correlated
            uintptr_t index = _sparse[key];
            uintptr_t dense_size = _dense.size();
            if (index == -1 || index >= dense_size || _dense[index] != id) {
                return;
            }

            // Swap last element of dense and pool array to maintain contiguity
            uintptr_t new_key = reinterpret_cast<uintptr_t>(_dense.back());
            std::swap(_pool.back(), _pool[index]);
            _pool.pop_back();

            _dense[index] = _dense.back();
            _dense.pop_back();

            // Update sparse set, pointing to newly swapped object
            _sparse[new_key] = index;
            _sparse[key] = -1;
        }

        /**
         * @brief Destroy all elements in the pool, emptying the sparse set.
         *
         */
        inline void clear() {
            _pool.clear();
            _dense.clear();
            _sparse.clear();
        }

        /**
         * @brief Get the item stored at an index.
         *
         * @param index Index position of the object within the pool array.
         * @return T&
         */
        inline T &operator[](unsigned index) {
            DYN_ASSERT(index >= 0);
            DYN_ASSERT(index < _pool.size());
            return _pool[index];
        }

        /**
         * @brief Get the item stored at an index.
         *
         * @param index Index position of the object within the pool array.
         * @return const T&
         */
        inline const T &operator[](unsigned index) const {
            DYN_ASSERT(index >= 0);
            DYN_ASSERT(index < _pool.size());
            return _pool[index];
        }

        /**
         * @brief Get the item associated with an id.
         *
         * @param id Unique identifier of the object to be queried.
         * @return T&
         */
        inline T &get(Id id) { return (*this)[find(id)]; }

        /**
         * @brief Get the item associated with an id.
         *
         * @param id Unique identifier of the object to be queried.
         * @return const T&
         */
        inline const T &get(Id id) const { return (*this)[find(id)]; }

        /**
         * @brief Get begin iterator to the dense array.
         *
         * @return T*
         */
        inline auto begin() { return _pool.begin(); }

        /**
         * @brief Get end iterator to the dense array.
         *
         * @return T*
         */
        inline auto end() { return _pool.end(); }

        /**
         * @brief Apply a function to each member of the set.
         *
         * @tparam Functor
         * @param function Function that takes a T object and its id.
         */
        template <typename Functor>
        inline void foreach (Functor &&function) {
            for (unsigned i = 0; i < size(); i++) {
                function(_pool[i], _dense[i]);
            }
        }

        /**
         * @brief Apply a function to each item in the set.
         *
         * @tparam Functor
         * @param function Function that takes a T object.
         */
        template <typename Functor>
        inline void foreach_items(Functor &&function) {
            for (unsigned i = 0; i < size(); i++) {
                function(_pool[i]);
            }
        }

        /**
         * @brief Apply a function to each id in the set.
         *
         * @tparam Functor
         * @param function Function that takes an id.
         */
        template <typename Functor>
        inline void foreach_ids(Functor &&function) {
            for (unsigned i = 0; i < size(); i++) {
                function(_dense[i]);
            }
        }
    };
} // namespace Dynamo