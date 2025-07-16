#pragma once

#include "Buffer.hpp"
#include "LootTable.hpp"
#include "common/Pos2DTemplate.hpp"
#include "terrain/Chunk.hpp"
#include "structures/gen/stronghold/stronghold.hpp"

namespace loot {

    /**
 * @brief A helper boolean for static assertions.
 */
    template<typename...>
    inline constexpr bool dependent_false_v = false;

    /**
     * @brief Helper function to unroll a loop at compile time.
     *
     * @tparam Is An index sequence.
     * @tparam Func The function type to call.
     * @param f The function to invoke with each unrolled index.
     */
    template<std::size_t... Is, typename Func>
    static constexpr void unrollLoopHelper(std::index_sequence<Is...>, Func &&f) {
        ((void) f(std::integral_constant<std::size_t, Is>{}), ...);
    }

    /**
     * @brief Unrolls a loop at compile time.
     *
     * @tparam N The number of iterations.
     * @tparam Func The function type to call.
     * @param f The function to be invoked with each index.
     */
    template<std::size_t N, typename Func>
    static constexpr void unrollLoop(Func &&f) {
        unrollLoopHelper(std::make_index_sequence<N>{}, std::forward<Func>(f));
    }

    /**
     * @brief Constructs a Loot object from the provided loot tables.
     *
     * @tparam ContainerSize The container size for generated loot.
     * @tparam Tables The loot table types.
     * @param ts The loot table instances.
     *
     * @details This constructor is evaluated at compile time and initializes the internal tuple
     * and computes the maximum item count.
     */
    template<size_t ContainerSize, bool Aquatic, typename... Tables>
    consteval Loot<ContainerSize, Aquatic, Tables...>::Loot(Tables... ts) noexcept
        : m_LootTables(ts...) {}

    /**
     * @brief Computes the maximum number of items that can be generated across all loot tables.
     *
     * @return The sum of the maximum item counts from each loot table.
     *
     * @details Uses a lambda with std::apply to iterate over the tuple of loot tables.
     */
    template<size_t ContainerSize, bool Aquatic, typename... Tables>
    consteval i32 Loot<ContainerSize, Aquatic, Tables...>::computeMaxItemCount() const noexcept {
        return std::apply([](auto const &...table) -> i32 {
            return (0 + ... + table.maxItemCount());
        },
                          m_LootTables);
    }

    /**
     * @brief Generates loot from a seed.
     *
     * @tparam Mode The specified loot generation mode (legacy / modern / modern shuffle)
     * @param container The container to fill with generated loot.
     * @param seed An RNG instance providing the seed.
     *
     * @details Calls getLootFromLootTableSeed using a seed obtained by calling seed.nextLong().
     */
    template<size_t ContainerSize, bool Aquatic, typename... Tables>
    template<GenMode Mode>
    MU void Loot<ContainerSize, Aquatic, Tables...>::getLootFromSeed(MU Container<ContainerSize> &container, RNG &seed) const {
        getLootFromLootTableSeed<Mode>(container, seed.nextLong());
    }

    /**
     * @brief Processes loot generation for legacy loot tables.
     *
     * @tparam ContainerSize The size of the container.
     * @param containerIn The container to store generated loot.
     * @param rng An RNG instance for randomization.
     * @param table A loot table to process.
     *
     * @details For legacy loot, this function generates loot items according to the roll count and
     * places them at random positions in the container.
     */
    template<size_t ContainerSize>
    static void NOINLINE processLootTableLegacy(
            Container<ContainerSize> &containerIn, RNG &rng, const auto &table) {
        static constexpr bool Legacy = true;
        const i32 rollCount = table.template getRollCount<Legacy>(rng);
        for (int rollIndex = 0; rollIndex < rollCount; rollIndex++) {
            lce::ItemState result = table.template rollLootItem<ContainerSize, Legacy>(containerIn, rng);
            containerIn.setSlot(rng.nextInt(ContainerSize), result);
        }
    }

    /**
     * @brief Processes loot generation for modern loot tables.
     *
     * @tparam ContainerSize The size of the container.
     * @param containerIn The container to be filled with generated loot.
     * @param rng An RNG instance.
     * @param table A loot table to process.
     *
     * @details Depending on the loot table type, this function either directly adds an item,
     * unrolls a loop to add multiple items, or iterates over a roll count to add items.
     */
    template<size_t ContainerSize>
    static void NOINLINE processLootTableModern(
            Container<ContainerSize> &containerIn, RNG &rng, const auto &table) {
        using table_t = std::decay_t<decltype(table)>;
        static constexpr bool Legacy = false;
        switch (table_t::k_TableType) {
            case LootTableType::RETURNS_1: {
                containerIn.addItem(table.template rollLootItem<ContainerSize, Legacy>(containerIn, rng));
                break;
            }
            case LootTableType::RETURNS_N: {
                unrollLoop<table_t::k_MinRoll>([&](auto) {
                    containerIn.addItem(table.template rollLootItem<ContainerSize, Legacy>(containerIn, rng));
                });
                break;
            }
            case LootTableType::RETURNS_R: {
                const i32 rollCount = table.template getRollCount<Legacy>(rng);
                for (int i = 0; i < rollCount; i += 1) {
                    auto item = table.template rollLootItem<ContainerSize, Legacy>(containerIn, rng);
                    containerIn.addItem(item);
                }
                break;
            }
            default:
                std::cout << "error!\n";
        }
    }

    /**
     * @brief Generates loot using a loot table seed.
     *
     * @tparam Mode The specified loot generation mode (legacy / modern / modern shuffle)
     * @param container The container to populate with loot.
     * @param lootTableSeed The seed used for loot table generation.
     * @param buffer A pointer to a temporary @c Buffer for shuffling.
     *
     * @details
     * The function sets the RNG seed based on @c lootTableSeed and then processes each loot table.
     * For legacy loot, it uses processLootTableLegacy; for modern loot, it uses processLootTableModern.
     * If shuffling is enabled (Shuffle == true), the container's items are shuffled using the provided buffer.
     *
     * @note Shuffling and legacy mode cannot be used simultaneously.
     */
    template<size_t ContainerSize, bool Aquatic, typename... Tables>
    template<GenMode Mode>
    MU void Loot<ContainerSize, Aquatic, Tables...>::getLootFromLootTableSeed(
        Container<ContainerSize> &container, c_i64 lootTableSeed, Buffer *buffer) const {

        RNG rng;
        rng.setSeed(lootTableSeed);

        if constexpr (Mode == GenMode::LEGACY) {
            std::apply([&](auto const &...table) {
                (processLootTableLegacy<ContainerSize>(container, rng, table), ...);
            },
                       m_LootTables);
        } else {
            std::apply([&](auto const &...table) {
                (processLootTableModern<ContainerSize>(container, rng, table), ...);
            },
                       m_LootTables);

            if constexpr (Mode == GenMode::MODERN) {
                if (buffer == nullptr) {
                    Buffer tempBuffer;
                    container.shuffleInto(rng, &tempBuffer);
                } else {
                    container.shuffleInto(rng, buffer);
                }
            }
        }
    }

    /// A helper variable template to check if all types are integral.
    template<typename... T>
    constexpr bool all_integral_v = (std::is_integral_v<std::decay_t<T>> && ...);

    /**
     * @brief Generates loot from block coordinates.
     *
     * @tparam Mode The specified loot generation mode (legacy / modern / modern shuffle)
     * @tparam Args Block coordinate representation (either two integers or a Pos2D object).
     * @param container The container to populate with loot.
     * @param worldSeed The world seed.
     * @param args Block coordinates.
     *
     * @details
     * This function dispatches to @c getLootFromChunk based on whether the block coordinates
     * are provided as two integers (blockX, blockZ) or a single @c Pos2D object.
     *
     * @warning Passing invalid parameters will cause a compile-time error.
     */
    template<size_t ContainerSize, bool Aquatic, typename... Tables>
    template<GenMode Mode, typename... Args>
    MU void Loot<ContainerSize, Aquatic, Tables...>::getLootFromBlock(Container<ContainerSize>& container, c_i64 worldSeed, Args &&...args) const {

        // Create a tuple from the forwarded arguments once.
        auto argsTuple = std::forward_as_tuple(std::forward<Args>(args)...);

        if constexpr (sizeof...(Args) == 2 && all_integral_v<Args...>) {
            // When two integral values are passed (blockX, blockZ)
            const int blockX = std::get<0>(argsTuple);
            const int blockZ = std::get<1>(argsTuple);
            getLootFromChunk<Mode>(container, worldSeed, blockX >> 4, blockZ >> 4);
        } else if constexpr (sizeof...(Args) == 1) {
            // When a single argument is passed; check if it's of type Pos2D.
            using Arg0 = std::decay_t<decltype(std::get<0>(argsTuple))>;
            if constexpr (std::is_same_v<Arg0, Pos2D>) {
                const Pos2D& blockPos = std::get<0>(argsTuple);
                getLootFromChunk<Mode>(container, worldSeed, blockPos.x >> 4, blockPos.z >> 4);
            } else {
                static_assert(dependent_false_v<Args...>, "getLootFromBlock(...) called with invalid parameter type. "
                                                          "Pass either (blockX, blockZ) or (Pos2D).");
            }
        } else {
            static_assert(dependent_false_v<Args...>, "getLootFromBlock(...) called with invalid parameter quantity. "
                                                      "Pass either (blockX, blockZ) or (Pos2D).");
        }
    }

    /**
     * @brief Generates loot from chunk coordinates.
     *
     * @tparam Mode The specified loot generation mode (legacy / modern / modern shuffle)
     * @tparam Args Chunk coordinate representation (either two integers or a Pos2D object).
     * @param container The container to populate with loot.
     * @param worldSeed The world seed.
     * @param args Chunk coordinates.
     *
     * @details
     * This function determines the chunk coordinates either from two integers or a single @c Pos2D object,
     * obtains a seed using @c RNG::getPopulationSeed, and then calls @c getLootFromLootTableSeed to generate loot.
     *
     * @warning Passing invalid parameters will cause a compile-time error.
     */
    template<size_t ContainerSize, bool Aquatic, typename... Tables>
    template<GenMode Mode, typename... Args>
    MU void Loot<ContainerSize, Aquatic, Tables...>::getLootFromChunk(Container<ContainerSize>& container, c_i64 worldSeed, Args &&...args) const {
        auto argsTuple = std::forward_as_tuple(std::forward<Args>(args)...);

        if constexpr (sizeof...(Args) == 2 && all_integral_v<Args...>) {
            const int chunkX = std::get<0>(argsTuple);
            const int chunkZ = std::get<1>(argsTuple);
            RNG seed = RNG::getPopulationSeed(worldSeed, chunkX, chunkZ);
            if constexpr (Aquatic) {
                getLootFromLootTableSeed<Mode>(container, (int)seed.nextLong());
            } else {
                getLootFromLootTableSeed<Mode>(container, seed.nextLong());
            }
        } else if constexpr (sizeof...(Args) == 1) {
            using Arg0 = std::decay_t<decltype(std::get<0>(argsTuple))>;
            if constexpr (std::is_same_v<Arg0, Pos2D>) {
                const Pos2D& chunkPos = std::get<0>(argsTuple);
                const int chunkX = chunkPos.x;
                const int chunkZ = chunkPos.z;
                RNG seed = RNG::getPopulationSeed(worldSeed, chunkX, chunkZ);
                if constexpr (Aquatic) {
                    getLootFromLootTableSeed<Mode>(container, (int)seed.nextLong());
                } else {
                    getLootFromLootTableSeed<Mode>(container, seed.nextLong());
                }
            } else {
                static_assert(dependent_false_v<Args...>, "getLootFromChunk(...) called with invalid parameter type. "
                                                          "Pass either (chunkX, chunkZ) or (Pos2D).");
            }
        } else {
            static_assert(dependent_false_v<Args...>, "getLootFromChunk(...) called with invalid parameter quantity. "
                                                      "Pass either (chunkX, chunkZ) or (Pos2D).");
        }
    }

    /// loot seeding with stronghold stone rolls
    template<size_t ContainerSize, bool Aquatic, typename... Tables>
    template<bool checkCaves, bool checkWaterCaves>
    RNG Loot<ContainerSize, Aquatic, Tables...>::getStrongholdLootSeed(const Generator& g, gen::Stronghold* strongholdGenerator,
                                        const StructureComponent& piece, c_int chestChunkX, c_int chestChunkZ,
                                        bool accurate) const {
         RNG rng = RNG::getPopulationSeed(g.getWorldSeed(), chestChunkX, chestChunkZ);

         if constexpr (checkCaves) {
             // TODO: probably needs fixed
             ChunkPrimer* chunk = Chunk::provideNewChunk(g, {chestChunkX, chestChunkZ}); // , accurate
             // we roll rng equal to the stone bricks in the chunk that generated before the chest corridor
             if (!rolls::Stronghold::generateStructure<true>(chunk, strongholdGenerator, rng, chestChunkX, chestChunkZ,
                                                             piece)) {
                 delete chunk;
                 return RNG::ConstructWithoutSetSeed(-1);
             }
             delete chunk;
         } else {
             rolls::Stronghold::generateStructure<true>(nullptr, strongholdGenerator, rng, chestChunkX, chestChunkZ,
                                                        piece);
         }
         return rng;
     }

    /// loot seeding with stronghold stone rolls
    template<size_t ContainerSize, bool Aquatic, typename... Tables>
    template<bool checkCaves, bool shuffle>
    Container27 Loot<ContainerSize, Aquatic, Tables...>::getStrongholdLoot(const Generator& g, gen::Stronghold* strongholdGenerator,
                                          const StructureComponent& piece, c_int chestChunkX, c_int chestChunkZ,
                                          const bool accurate) const {
         u64 lootSeed = getStrongholdLootSeed<checkCaves>(g, strongholdGenerator, piece, chestChunkX,
                                                                            chestChunkZ, accurate);
         if (lootSeed == -1ULL) return {};
         return getLootFromSeed<shuffle>(lootSeed);
     }

    /**
     * @brief Helper structure to aggregate multiple loot tables into a single loot instance.
     *
     * @tparam ContainerSize The size of the container for generated loot.
     * @tparam Wrappers One or more wrappers containing loot tables.
     *
     * @details
     * The @c LootWrapper structure creates a compile-time constant instance of @c Loot by aggregating
     * the loot tables provided in the wrappers. This simplifies usage when referencing a combined loot configuration.
     */
    template<size_t ContainerSize, bool Aquatic, typename... Wrappers>
    struct LootWrapper {
        using loot_t = Loot<ContainerSize, Aquatic, typename Wrappers::table_t...>;
        /// A compile-time constant aggregated loot instance.
        static constexpr loot_t value{Wrappers::value...};
    };

} // namespace loot
