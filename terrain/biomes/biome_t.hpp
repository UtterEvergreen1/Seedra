#pragma once

#include <cstdint>

#define biome_u int




enum class land_mask : uint8_t {
    ocean = 0,
    land = 1
};




/**
 * @enum BiomeTempCategory
 * @brief Represents the temperature categories for biomes.
 */
enum biome_category_t : uint8_t {
    oceanic,  ///< Biomes with oceanic temperature.
    warm,     ///< Biomes with warm temperature.
    lush,     ///< Biomes with lush temperature.
    cold,     ///< Biomes with cold temperature.
    freezing, ///< Biomes with freezing temperature.
    special   ///< Special biomes with unique temperature characteristics.
};



/**
 * @enum BiomeID
 * @brief Represents the unique identifiers for different biomes.
 */
enum biome_t : biome_u {
    none = 255, ///< No biome.
    // 0
    ocean = 0, ///< Ocean biome.
    plains, ///< Plains biome.
    desert, ///< Desert biome.
    extreme_hills, ///< Extreme hills biome.
    mountains = extreme_hills, ///< Alias for extreme hills biome.
    forest, ///< Forest biome.
    taiga, ///< Taiga biome.
    swampland, ///< Swampland biome.
    swamp = swampland, ///< Alias for swampland biome.
    river, ///< River biome.
    hell, ///< Hell biome (Nether).
    nether_wastes = hell, ///< Alias for Nether wastes biome.
    the_end, ///< The End biome.
    // 10
    legacy_frozen_ocean, ///< Legacy frozen ocean biome.
    frozenOcean = legacy_frozen_ocean, ///< Alias for legacy frozen ocean biome.
    frozen_river, ///< Frozen river biome.
    ice_plains, ///< Ice plains biome.
    snowy_tundra = ice_plains, ///< Alias for snowy tundra biome.
    ice_mountains, ///< Ice mountains biome.
    snowy_mountains = ice_mountains, ///< Alias for snowy mountains biome.
    mushroom_island, ///< Mushroom island biome.
    mushroom_fields = mushroom_island, ///< Alias for mushroom fields biome.
    mushroom_island_shore, ///< Mushroom island shore biome.
    mushroom_field_shore = mushroom_island_shore, ///< Alias for mushroom field shore biome.
    beach, ///< Beach biome.
    desert_hills, ///< Desert hills biome.
    forest_hills, ///< Forest hills biome.
    wooded_hills = forest_hills, ///< Alias for forest hills biome.
    taiga_hills, ///< Taiga hills biome.
    // 20
    extreme_hills_edge, ///< Extreme hills edge biome.
    mountain_edge = extreme_hills_edge, ///< Alias for mountain edge biome.
    jungle, ///< Jungle biome.
    jungle_hills, ///< Jungle hills biome.
    jungle_edge, ///< Jungle edge biome.
    deep_ocean, ///< Deep ocean biome.
    stone_beach, ///< Stone beach biome.
    stone_shore = stone_beach, ///< Alias for stone shore biome.
    cold_beach, ///< Cold beach biome.
    snowy_beach = cold_beach, ///< Alias for snowy beach biome.
    birch_forest, ///< Birch forest biome.
    birch_forest_hills, ///< Birch forest hills biome.
    roofed_forest, ///< Roofed forest biome.
    dark_forest = roofed_forest, ///< Alias for dark forest biome.
    // 30
    cold_taiga, ///< Cold taiga biome.
    snowy_taiga = cold_taiga, ///< Alias for snowy taiga biome.
    cold_taiga_hills, ///< Cold taiga hills biome.
    snowy_taiga_hills = cold_taiga_hills, ///< Alias for snowy taiga hills biome.
    mega_taiga, ///< Mega taiga biome.
    giant_tree_taiga = mega_taiga, ///< Alias for giant tree taiga biome.
    mega_taiga_hills, ///< Mega taiga hills biome.
    giant_tree_taiga_hills = mega_taiga_hills, ///< Alias for giant tree taiga hills biome.
    extreme_hills_plus_trees, ///< Extreme hills with trees biome.
    wooded_mountains = extreme_hills_plus_trees, ///< Alias for wooded mountains biome.
    savanna, ///< Savanna biome.
    savanna_plateau, ///< Savanna plateau biome.
    mesa, ///< Mesa biome.
    badlands = mesa, ///< Alias for badlands biome.
    mesa_plateau_stone, ///< Mesa plateau stone biome.
    wooded_badlands_plateau = mesa_plateau_stone, ///< Alias for wooded badlands plateau biome.
    mesa_plateau, ///< Mesa plateau biome.
    badlands_plateau = mesa_plateau, ///< Alias for badlands plateau biome.
    // 40  --  1.13
    warm_ocean, ///< Warm ocean biome.
    deep_warm_ocean, ///< Deep warm ocean biome.
    lukewarm_ocean, ///< Lukewarm ocean biome.
    deep_lukewarm_ocean, ///< Deep lukewarm ocean biome.
    cold_ocean, ///< Cold ocean biome.
    deep_cold_ocean, ///< Deep cold ocean biome.
    frozen_ocean, ///< Frozen ocean biome.
    deep_frozen_ocean, ///< Deep frozen ocean biome.
    small_end_islands, ///< Small End islands biome.
    end_midlands, ///< End midlands biome.
    end_highlands, ///< End highlands biome.
    end_barrens, ///< End barrens biome.
    // 50
    // BIOME_NUM,

    the_void = 127, ///< The Void biome.

    // mutated variants
    sunflower_plains = plains + 128, ///< Sunflower plains biome.
    desert_mutated = desert + 128, ///< Mutated desert biome.
    desert_lakes = desert_mutated, ///< Alias for desert lakes biome.
    extreme_hills_mutated = extreme_hills + 128, ///< Mutated extreme hills biome.
    gravelly_mountains = extreme_hills_mutated, ///< Alias for gravelly mountains biome.
    flower_forest = forest + 128, ///< Flower forest biome.
    taiga_mutated = taiga + 128, ///< Mutated taiga biome.
    taiga_mountains = taiga_mutated, ///< Alias for taiga mountains biome.
    swampland_mutated = swampland + 128, ///< Mutated swampland biome.
    swamp_hills = swampland_mutated, ///< Alias for swamp hills biome.
    ice_plains_spikes = ice_plains + 128, ///< Ice plains spikes biome.
    ice_spikes = ice_plains_spikes, ///< Alias for ice spikes biome.
    jungle_mutated = jungle + 128, ///< Mutated jungle biome.
    modified_jungle = jungle_mutated, ///< Alias for modified jungle biome.
    jungle_edge_mutated = jungle_edge + 128, ///< Mutated jungle edge biome.
    modified_jungle_edge = jungle_edge_mutated, ///< Alias for modified jungle edge biome.
    birch_forest_mutated = birch_forest + 128, ///< Mutated birch forest biome.
    tall_birch_forest = birch_forest_mutated, ///< Alias for tall birch forest biome.
    birch_forest_hills_mutated = birch_forest_hills + 128, ///< Mutated birch forest hills biome.
    tall_birch_hills = birch_forest_hills_mutated, ///< Alias for tall birch hills biome.
    roofed_forest_mutated = roofed_forest + 128, ///< Mutated roofed forest biome.
    dark_forest_hills = roofed_forest_mutated, ///< Alias for dark forest hills biome.
    cold_taiga_mutated = cold_taiga + 128, ///< Mutated cold taiga biome.
    snowy_taiga_mountains = cold_taiga_mutated, ///< Alias for snowy taiga mountains biome.
    mega_spruce_taiga = mega_taiga + 128, ///< Mega spruce taiga biome.
    giant_spruce_taiga = mega_spruce_taiga, ///< Alias for giant spruce taiga biome.
    redwood_taiga_hills_mutated = mega_taiga_hills + 128, ///< Mutated redwood taiga hills biome.
    giant_spruce_taiga_hills = redwood_taiga_hills_mutated, ///< Alias for giant spruce taiga hills biome.
    extreme_hills_plus_trees_mutated = extreme_hills_plus_trees + 128, ///< Mutated extreme hills with trees biome.
    modified_gravelly_mountains = extreme_hills_plus_trees_mutated, ///< Alias for modified gravelly mountains biome.
    savanna_mutated = savanna + 128, ///< Mutated savanna biome.
    shattered_savanna = savanna_mutated, ///< Alias for shattered savanna biome.
    savanna_plateau_mutated = savanna_plateau + 128, ///< Mutated savanna plateau biome.
    shattered_savanna_plateau = savanna_plateau_mutated, ///< Alias for shattered savanna plateau biome.
    mesa_bryce = mesa + 128, ///< Mesa Bryce biome.
    eroded_badlands = mesa_bryce, ///< Alias for eroded badlands biome.
    mesa_plateau_stone_mutated = mesa_plateau_stone + 128, ///< Mutated mesa plateau stone biome.
    modified_wooded_badlands_plateau = mesa_plateau_stone_mutated, ///< Alias for modified wooded badlands plateau biome.
    mesa_plateau_mutated = mesa_plateau + 128, ///< Mutated mesa plateau biome.
    modified_badlands_plateau = mesa_plateau_mutated, ///< Alias for modified badlands plateau biome.
    // 1.14
    bamboo_jungle = 168, ///< Bamboo jungle biome.
    bamboo_jungle_hills = 169 ///< Bamboo jungle hills biome.
};
