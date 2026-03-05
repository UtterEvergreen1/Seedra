#pragma once

#include "Buffer.hpp"
#include "Container.hpp"
#include "LootGenMode.hpp"
#include "LootItem.hpp"
#include "lce/processor.hpp"

class Generator;

namespace gen {
    class Stronghold;
}
class StructureComponent;

namespace loot {

    /**
     * @brief A constexpr-safe alternative to std::tuple that avoids libc++ EBO/constexpr bugs.
     *
     * Stores each element in a separate base class indexed by position, so the compiler
     * never sees two bases of the same type even when Tables... are instantiated from the
     * same template.
     */
    template<std::size_t I, typename T>
    struct PackedTupleLeaf {
        T value;
        consteval explicit PackedTupleLeaf(const T& v) noexcept : value(v) {}
    };

    template<typename Indices, typename... Ts>
    struct PackedTupleImpl;

    template<std::size_t... Is, typename... Ts>
    struct PackedTupleImpl<std::index_sequence<Is...>, Ts...>
        : PackedTupleLeaf<Is, Ts>... {
        consteval explicit PackedTupleImpl(const Ts&... vs) noexcept
            : PackedTupleLeaf<Is, Ts>(vs)... {}
    };

    template<typename... Ts>
    struct PackedTuple : PackedTupleImpl<std::index_sequence_for<Ts...>, Ts...> {
        consteval explicit PackedTuple(const Ts&... vs) noexcept
            : PackedTupleImpl<std::index_sequence_for<Ts...>, Ts...>(vs...) {}

        template<typename Func>
        constexpr void forEach(Func&& f) const {
            [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                (f(static_cast<const PackedTupleLeaf<Is, Ts>&>(*this).value), ...);
            }(std::index_sequence_for<Ts...>{});
        }
    };

    /**
     * @brief Aggregates multiple loot tables to generate loot.
     *
     * @tparam ContainerSize The size of the container for generated loot.
     * @tparam Tables One or more loot table types.
     *
     * The Loot class holds a tuple of loot tables and provides functions to generate loot
     * from a seed, chunk coordinates, block coordinates, or a loot table seed. Detailed
     * function documentation is provided in the corresponding implementation file (Loot.inl).
     */
    template<size_t ContainerSize, bool Aquatic, typename... Tables>
    class Loot final {

        /**
         * @brief Tuple containing the loot tables.
         *
         * This tuple stores the loot tables provided during construction. Each element in the tuple
         * represents a loot table that will be used during loot generation.
         */
        const PackedTuple<Tables...> m_LootTables;

        ND static consteval i32 computeMaxItemCount() noexcept;

        /**
         * @brief The maximum number of items that can be generated.
         *
         * This compile-time constant is computed as the sum of maximum item counts from all loot tables.
         * It is calculated by the private member function @c computeMaxItemCount().
         */
#if defined(_MSC_VER) || defined(__EMSCRIPTEN__)
        static constexpr i32 MAX_ITEM_COUNT = 27;
#else
        static constexpr i32 MAX_ITEM_COUNT = computeMaxItemCount();
#endif
    public:
        consteval Loot() = delete;
        consteval explicit Loot(Tables... ts) noexcept;

        template<GenMode Mode>
        MU void getLootFromSeed(Container<ContainerSize>& container, RNG& seed) const;

        template<GenMode Mode, typename... Args>
        MU void getLootFromChunk(Container<ContainerSize>& container, i64 worldSeed, Args &&...args) const;

        template<GenMode Mode, typename... Args>
        MU void getLootFromBlock(Container<ContainerSize>& container, i64 worldSeed, Args &&...args) const;

        template<GenMode Mode>
        MU void getLootFromLootTableSeed(Container<ContainerSize>& container, c_i64 lootTableSeed, Buffer* buffer = nullptr) const;

        /// loot seeding with stronghold stone rolls
        template<bool checkCaves, bool checkWaterCaves = false>
        ND RNG getStrongholdLootSeed(const Generator& g, gen::Stronghold* strongholdGenerator,
                                  const StructureComponent& piece, c_int chestChunkX, c_int chestChunkZ, bool accurate) const;

        /// combine loot seeding and generation to get the stronghold loot
        template<bool checkCaves, bool shuffle>
        MU ND Container27 getStrongholdLoot(const Generator& g, gen::Stronghold* strongholdGenerator,
                                       const StructureComponent& piece, c_int chestChunkX, c_int chestChunkZ,
                                       bool accurate) const;
    };

} // namespace loot

#include "Loot.inl"
