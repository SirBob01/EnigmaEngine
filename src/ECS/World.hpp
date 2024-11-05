#pragma once

#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <ECS/Component.hpp>
#include <ECS/SparsePool.hpp>
#include <Utils/SparseArray.hpp>

namespace Dynamo::ECS {
    /**
     * @brief Entity-component-system world.
     *
     */
    class World {
        std::vector<SparsePool> _pools;

        uintptr_t _counter;
        std::vector<Entity> _recycle;

        // Owned component indices
        std::unordered_map<Entity, std::unordered_set<unsigned>> _owned;

        template <typename Component, typename Min>
        Component &fast_get(Entity entity, unsigned index) {
            unsigned id = ComponentRegistry::get<Component>();
            if constexpr (std::is_same_v<Component, Min>) {
                return _pools[id].get<Component>(index);
            } else {
                return _pools[id].get<Component>(entity);
            }
        };

        template <typename Min,
                  typename Functor,
                  typename... Include,
                  template <typename...> class I = Group,
                  typename... Exclude,
                  template <typename...> class E = Group>
        void iterate_group(Functor &function, SparsePool &min, I<Include...> &, E<Exclude...> &) {
            unsigned index = 0;
            for (Entity entity : min.dense()) {
                if ((_pools[ComponentRegistry::get<Include>()].exists(entity) && ...) &&
                    (!_pools[ComponentRegistry::get<Exclude>()].exists(entity) && ...)) {
                    function(entity, fast_get<Include, Min>(entity, index)...);
                }
                index++;
            }
        }

        template <typename Component>
        unsigned get_pool_id() {
            unsigned id = ComponentRegistry::get<Component>();
            if (id >= _pools.size()) {
                _pools.resize(id + 1);
            }
            if (_pools[id].invalid()) {
                _pools[id].initialize(sizeof(Component));
            }
            return id;
        }

      public:
        /**
         * @brief Construct a new ECS World.
         *
         */
        World() : _counter(0) {}

        /**
         * @brief Create a new entity.
         *
         * @return Entity
         */
        Entity create() {
            Entity entity;
            if (_recycle.size()) {
                entity = _recycle.back();
                _recycle.pop_back();
            } else {
                entity = reinterpret_cast<Entity>(_counter++);
                _owned[entity];
            }
            return entity;
        }

        /**
         * @brief Destroy an entity.
         *
         * @param entity
         */
        void destroy(Entity entity) {
            auto id_it = _owned.find(entity);
            if (id_it != _owned.end()) {
                for (unsigned id : id_it->second) {
                    _pools[id].remove(entity);
                }
                id_it->second.clear();
                _recycle.push_back(entity);
            }
        }

        /**
         * @brief Get a component from an entity.
         *
         * @tparam Component
         * @param entity
         * @return Component&
         */
        template <typename Component>
        Component &get(Entity entity) {
            unsigned id = get_pool_id<Component>();
            return _pools[id].get<Component>(entity);
        }

        /**
         * @brief Safely get a component from an entity, returning an empty value if one does not exist.
         *
         * Do not use this if possible, runtime checks are more costly.
         *
         * @tparam Component
         * @param entity
         * @return std::optional<std::reference_wrapper<Component>>
         */
        template <typename Component>
        std::optional<std::reference_wrapper<Component>> get_safe(Entity entity) {
            unsigned id = get_pool_id<Component>();
            if (_pools[id].exists(entity)) {
                return _pools[id].get<Component>(entity);
            }
            return {};
        }

        /**
         * @brief Add a component to an entity.
         *
         * @tparam Component
         * @tparam Fields
         * @param entity
         * @param params
         */
        template <typename Component, typename... Params>
        void add(Entity entity, Params... args) {
            if constexpr (std::is_aggregate_v<Component>) {
                add(entity, {args...});
            } else {
                add(entity, Component(args...));
            }
        }

        /**
         * @brief Add a component to an entity.
         *
         * @tparam Component
         * @tparam Fields
         * @param entity
         * @param params
         */
        template <typename Component>
        void add(Entity entity, Component &&component) {
            unsigned id = get_pool_id<Component>();
            _pools[id].insert(entity, component);
            _owned.find(entity)->second.insert(id);
        }

        /**
         * @brief Remove a component from an entity.
         *
         * @tparam Component
         * @param entity
         */
        template <typename Component>
        void remove(Entity entity) {
            unsigned id = get_pool_id<Component>();
            _pools[id].remove(entity);
            _owned.find(entity)->second.erase(id);
        }

        /**
         * @brief Iterate over a component pool.
         *
         * @tparam Component
         * @tparam Functor
         * @param function
         */
        template <typename Component, typename Functor>
        void foreach (Functor function) {
            unsigned id = get_pool_id<Component>();
            _pools[id].foreach<Component>(function);
        }

        /**
         * @brief Iterate over a group of components.
         *
         * An exclusion group can be provided to filter entities with certain components.
         *
         * @tparam Include
         * @tparam Exclude
         * @tparam Functor
         * @param function
         * @param exclude
         */
        template <typename... Include, typename... Exclude, template <typename...> class E = Group, typename Functor>
        void foreach_group(Functor function, const E<Exclude...> &exclude = Group<>{}) {
            ((get_pool_id<Exclude>()), ...);
            Group<Include...> include_group;
            Group<Exclude...> exclude_group;
            SparsePool *min = std::min({&_pools[get_pool_id<Include>()]...},
                                       [](SparsePool *a, SparsePool *b) { return a->size() < b->size(); });
            ((&_pools[ComponentRegistry::get<Include>()] == min
                  ? iterate_group<Include>(function, *min, include_group, exclude_group)
                  : void()),
             ...);
        };

        /**
         * @brief Clear a component pool.
         *
         * @tparam Component
         */
        template <typename Component>
        void clear() {
            unsigned id = get_pool_id<Component>();
            _pools[id].clear();
        }

        /**
         * @brief Clear all component pools.
         *
         */
        void clear() { _pools.clear(); }
    };
} // namespace Dynamo::ECS