#include "EnchantmentProtection.hpp"

const EnchantmentProtection::ArmorType EnchantmentProtection::ALL = {"all", 0, 1, 11, 20};
const EnchantmentProtection::ArmorType EnchantmentProtection::FIRE = {"fire", 1, 10, 8, 12};
const EnchantmentProtection::ArmorType EnchantmentProtection::FALL = {"fall", 2, 5, 6, 10};
const EnchantmentProtection::ArmorType EnchantmentProtection::EXPLOSION = {"explosion", 3, 5, 8, 12};
const EnchantmentProtection::ArmorType EnchantmentProtection::PROJECTILE = {"projectile", 4, 3, 6, 15};

/*
typeName          fall
id                   2
minEnchantability    5
levelCost            6
levelCostSpan       10
 */