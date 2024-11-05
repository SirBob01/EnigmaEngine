#pragma once

namespace Dynamo::ECS {
    /**
     * @brief Grouping of component types.
     *
     * @tparam Components
     */
    template <typename... Components>
    struct Group {};

    /**
     * @brief Generate a unique identifier for a component type at runtime.
     *
     */
    class ComponentRegistry {
        static inline unsigned _counter = 0;

      public:
        /**
         * @brief Get the unique identifier of a type.
         *
         * @tparam Type.
         * @return unsigned
         */
        template <typename Type>
        static inline unsigned get() {
            static const unsigned id = _counter++;
            return id;
        }
    };
} // namespace Dynamo::ECS