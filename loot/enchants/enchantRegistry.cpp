#include "enchantRegistry.hpp"

namespace enchants {

    /**
     * @brief Retrieves the global lookup table for ordering enchantments.
     *
     * @return A constant reference to a std::map mapping a KeyType_t (console/version pair) to a span
     * of enchantment IDs.
     *
     * @details
     * This static function defines the lookup table used to order enchantments based on the console and version.
     * Different arrays of enchantment IDs are specified for various combinations of console (e.g., XBOX360, WIIU)
     * and version (e.g., AQUATIC, ELYTRA). These arrays determine the order in which enchantments are considered active.
     * Aquatic: 33 enchants (total weight: 126 (including 0 and 126))
     * Elytra : 29 enchants (total weight: 108 (including 0 and 108))
     */
    const std::map<KeyType_t, std::span<const eEnchantId>>&
    EnchantRegistry::getTableOfOrders() {

        static constexpr eEnchantId XBOX360_AQUATIC[] = {
                channeling, riptide, impaling, lure, luck_of_the_sea, punch, fortune,
                silk_touch, looting, depth_strider, unbreaking, bane_of_arthropods, frost_walker, 
                projectile_protection, respiration, thorns, loyalty, infinity, power, efficiency, fire_aspect,
                smite, sharpness, protection, feather_falling, aqua_affinity, vanishing_curse, mending,
                flame, binding_curse, fire_protection, blast_protection, knockback
        };

        static constexpr eEnchantId XBOX360_ELYTRA[] = {
                lure, luck_of_the_sea, punch, fortune, silk_touch, looting, depth_strider,
                unbreaking, bane_of_arthropods, frost_walker, projectile_protection, respiration, thorns, 
                infinity, power, efficiency, fire_aspect, smite, sharpness, protection, feather_falling,
                aqua_affinity, vanishing_curse, mending, flame, binding_curse, fire_protection,
                blast_protection, knockback
        };


        // PS3 NPUB31419 01.83
        static constexpr eEnchantId PS3_AQUATIC[] = {
                channeling, loyalty, impaling,  power, vanishing_curse, feather_falling, respiration, 
                aqua_affinity, luck_of_the_sea, fire_aspect, infinity, bane_of_arthropods, flame, 
                punch, silk_touch, efficiency, looting, thorns, mending, lure, knockback, smite, 
                sharpness, frost_walker, protection, depth_strider, binding_curse, fortune, 
                projectile_protection, riptide, blast_protection, unbreaking, fire_protection
        };
        
        // PS3 NPEB01899 01.69
        static constexpr eEnchantId PS3_ELYTRA[] = {
                luck_of_the_sea, flame, fortune, unbreaking, silk_touch, efficiency, fire_protection, 
                bane_of_arthropods, punch, smite, depth_strider, sharpness, thorns, looting, 
                binding_curse, frost_walker, power, aqua_affinity, fire_aspect, respiration, 
                knockback, projectile_protection, infinity, blast_protection, lure, feather_falling, 
                vanishing_curse, mending, protection
        };
        

        static constexpr eEnchantId WIIU_AQUATIC[] = {
                channeling, impaling, vanishing_curse, infinity, fire_aspect, flame, punch,
                efficiency, projectile_protection, thorns, looting, knockback, bane_of_arthropods, luck_of_the_sea,
                protection, loyalty, aqua_affinity, sharpness, lure, power, frost_walker,
                fire_protection, respiration, binding_curse, depth_strider, fortune, mending, silk_touch,
                unbreaking, feather_falling, riptide, blast_protection, smite
        };

        static constexpr eEnchantId WIIU_ELYTRA[] = {
                mending, luck_of_the_sea, punch, fortune, unbreaking, efficiency, looting,
                smite, vanishing_curse, binding_curse, silk_touch, frost_walker, flame, depth_strider,
                knockback, bane_of_arthropods, respiration, thorns, sharpness, aqua_affinity, infinity,
                projectile_protection, blast_protection, lure, feather_falling, fire_aspect, fire_protection,
                power, protection
        };


        static const std::map<KeyType_t, std::span<const eEnchantId>> TABLE = {
                {{lce::CONSOLE::XBOX360, LCEVERSION::AQUATIC}, std::span<const eEnchantId>(XBOX360_AQUATIC)},
                {{lce::CONSOLE::XBOX360, LCEVERSION::ELYTRA}, std::span<const eEnchantId>(XBOX360_ELYTRA)},
                {{lce::CONSOLE::WIIU, LCEVERSION::AQUATIC}, std::span<const eEnchantId>(WIIU_AQUATIC)},
                {{lce::CONSOLE::WIIU, LCEVERSION::ELYTRA}, std::span<const eEnchantId>(WIIU_ELYTRA)},
                {{lce::CONSOLE::PS3, LCEVERSION::AQUATIC}, std::span<const eEnchantId>(PS3_AQUATIC)},
                {{lce::CONSOLE::PS3, LCEVERSION::ELYTRA}, std::span<const eEnchantId>(PS3_ELYTRA)}
        };
        return TABLE;
    }

} // namespace enchants
