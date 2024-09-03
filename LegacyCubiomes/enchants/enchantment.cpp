#include "enchantment.hpp"

#include "LegacyCubiomes/enchants/classes/EnchantmentArrowDamage.hpp"
#include "LegacyCubiomes/enchants/classes/EnchantmentArrowFire.hpp"
#include "LegacyCubiomes/enchants/classes/EnchantmentArrowInfinite.hpp"
#include "LegacyCubiomes/enchants/classes/EnchantmentArrowKnockback.hpp"
#include "LegacyCubiomes/enchants/classes/EnchantmentBindingCurse.hpp"
#include "LegacyCubiomes/enchants/classes/EnchantmentDamage.hpp"
#include "LegacyCubiomes/enchants/classes/EnchantmentDigging.hpp"
#include "LegacyCubiomes/enchants/classes/EnchantmentDurability.hpp"
#include "LegacyCubiomes/enchants/classes/EnchantmentFireAspect.hpp"
#include "LegacyCubiomes/enchants/classes/EnchantmentFishingSpeed.hpp"
#include "LegacyCubiomes/enchants/classes/EnchantmentFrostWalker.hpp"
#include "LegacyCubiomes/enchants/classes/EnchantmentKnockback.hpp"
#include "LegacyCubiomes/enchants/classes/EnchantmentLootBonus.hpp"
#include "LegacyCubiomes/enchants/classes/EnchantmentMending.hpp"
#include "LegacyCubiomes/enchants/classes/EnchantmentOxygen.hpp"
#include "LegacyCubiomes/enchants/classes/EnchantmentProtection.hpp"
#ifdef INCLUDE_JAVA
#include "LegacyCubiomes/enchants/classes/EnchantmentSweepingEdge.hpp"
#endif
#include "LegacyCubiomes/enchants/classes/EnchantmentThorns.hpp"
#include "LegacyCubiomes/enchants/classes/EnchantmentUntouching.hpp"
#include "LegacyCubiomes/enchants/classes/EnchantmentVanishingCurse.hpp"
#include "LegacyCubiomes/enchants/classes/EnchantmentWaterWalker.hpp"
#include "LegacyCubiomes/enchants/classes/EnchantmentWaterWorker.hpp"
#include "LegacyCubiomes/enchants/classes/EnchantmentTridentImpaler.hpp"
#include "LegacyCubiomes/enchants/classes/EnchantmentTridentRiptide.hpp"
#include "LegacyCubiomes/enchants/classes/EnchantmentTridentLoyalty.hpp"
#include "LegacyCubiomes/enchants/classes/EnchantmentTridentChanneling.hpp"


// TODO: FINISH TABLE (PS3 and older versions)
const std::map<lce::CONSOLE, std::vector<std::vector<int>>> Enchantment::tableOfOrders = {
    {lce::CONSOLE::XBOX360, {
        {
            channeling, riptide, impaling, lure, luck_of_the_sea, punch, fortune,
            silk_touch, looting, depth_strider, unbreaking, bane_of_arthropods, frost_walker, projectile_protection,
            respiration, thorns, loyalty, infinity, power, efficiency, fire_aspect,
            smite, sharpness, protection, feather_falling, aqua_affinity, vanishing_curse, mending,
            flame, binding_curse, fire_protection, blast_protection, knockback
        },
        {
            lure, luck_of_the_sea, punch, fortune, silk_touch, looting, depth_strider,
            unbreaking, bane_of_arthropods, frost_walker, projectile_protection, respiration, thorns, infinity,
            power, efficiency, fire_aspect, smite, sharpness, protection, feather_falling,
            aqua_affinity, vanishing_curse, mending, flame, binding_curse, fire_protection,
            blast_protection, knockback
        }
    }},
    {lce::CONSOLE::PS3, {
        {
            0, 1, 2, 3, 4, 5, 6,
            7, 8, 9, 10, 11, 12, 13,
            14, 15, 16, 17, 18, 19, 20,
            21, 22, 23, 24, 25, 26, 27,
            28, 29, 30, 31, 32
        },
        {
            0, 1, 2, 3, 4, 5, 6,
            7, 8, 9, 10, 11, 12, 13,
            14, 15, 16, 17, 18, 19, 20,
            21, 22, 23, 24, 25, 26, 27,
            28
        }
    }},
    {lce::CONSOLE::WIIU, {
        {
            channeling, impaling, vanishing_curse, infinity, fire_aspect, flame, punch,
            efficiency, projectile_protection, thorns, looting, knockback, bane_of_arthropods, luck_of_the_sea,
            protection, loyalty, aqua_affinity, sharpness, lure, power, frost_walker,
            fire_protection, respiration, binding_curse, depth_strider, fortune, mending, silk_touch,
            unbreaking, feather_falling, riptide, blast_protection, smite
        },
        {
            mending, luck_of_the_sea, punch, fortune, unbreaking, efficiency, looting,
            smite, vanishing_curse, binding_curse, silk_touch, frost_walker, flame, depth_strider,
            knockback, bane_of_arthropods, respiration, thorns, sharpness, aqua_affinity, infinity,
            projectile_protection, blast_protection, lure, feather_falling, fire_aspect, fire_protection,
            power, protection
        }
    }}
};


bool Enchantment::isSetup = false;
int Enchantment::count = 0;
bool Enchantment::Type::Base::canEnchantItem(const lce::items::Item *itemIn) const {
    return true;
}


bool Enchantment::Type::All::canEnchantItem(const lce::items::Item *itemIn) const {
    for (const Type::Base* enumType : ALL_ITERABLE) {
        if (enumType != &ALL && enumType->canEnchantItem(itemIn))
            return true;
    }
    return false;
}


bool      Enchantment::Type::Armor::canEnchantItem(const lce::items::Item *itemIn) const { return itemIn->getItemType() == lce::items::ItemType::ItemArmor; }
bool  Enchantment::Type::ArmorFeet::canEnchantItem(const lce::items::Item *itemIn) const { return itemIn->getArmorType() == lce::items::EntityEquipSlot::FEET; }
bool  Enchantment::Type::ArmorLegs::canEnchantItem(const lce::items::Item *itemIn) const { return itemIn->getArmorType() == lce::items::EntityEquipSlot::LEGS; }
bool Enchantment::Type::ArmorChest::canEnchantItem(const lce::items::Item *itemIn) const { return itemIn->getArmorType() == lce::items::EntityEquipSlot::CHEST; }
bool  Enchantment::Type::ArmorHead::canEnchantItem(const lce::items::Item *itemIn) const { return itemIn->getArmorType() == lce::items::EntityEquipSlot::HEAD; }
bool     Enchantment::Type::Weapon::canEnchantItem(const lce::items::Item *itemIn) const { return itemIn->getItemType() == lce::items::ItemType::ItemSword; }
bool     Enchantment::Type::Digger::canEnchantItem(const lce::items::Item *itemIn) const { return itemIn->getItemType() == lce::items::ItemType::ItemTool; }
bool Enchantment::Type::FishingRod::canEnchantItem(const lce::items::Item *itemIn) const { return itemIn->getItemType() == lce::items::ItemType::ItemFishingRod; }
bool  Enchantment::Type::Breakable::canEnchantItem(const lce::items::Item *itemIn) const { return itemIn->isDamageable(); }
bool        Enchantment::Type::Bow::canEnchantItem(const lce::items::Item *itemIn) const { return itemIn->getItemType() == lce::items::ItemType::ItemBow; }
bool    Enchantment::Type::Trident::canEnchantItem(const lce::items::Item *itemIn) const { return itemIn->getItemType() == lce::items::ItemType::ItemTrident; }


bool Enchantment::Type::Wearable::canEnchantItem(const lce::items::Item *itemIn) const {
    return itemIn->getArmorType() != lce::items::EntityEquipSlot::NONE
           || itemIn->getItemType() == lce::items::ItemType::ItemSkull
           || itemIn->getItemType() == lce::items::ItemType::ItemElytra;
}


const Enchantment::Type::All Enchantment::Type::ALL;
const Enchantment::Type::Armor Enchantment::Type::ARMOR;
const Enchantment::Type::ArmorFeet Enchantment::Type::ARMOR_FEET;
const Enchantment::Type::ArmorLegs Enchantment::Type::ARMOR_LEGS;
const Enchantment::Type::ArmorChest Enchantment::Type::ARMOR_CHEST;
const Enchantment::Type::ArmorHead Enchantment::Type::ARMOR_HEAD;
const Enchantment::Type::Weapon Enchantment::Type::WEAPON;
const Enchantment::Type::Digger Enchantment::Type::DIGGER;
const Enchantment::Type::FishingRod Enchantment::Type::FISHING_ROD;
const Enchantment::Type::Breakable Enchantment::Type::BREAKABLE;
const Enchantment::Type::Bow Enchantment::Type::BOW;
const Enchantment::Type::Wearable Enchantment::Type::WEARABLE;
const Enchantment::Type::Trident Enchantment::Type::TRIDENT;


std::vector<const Enchantment::Type::Base*> Enchantment::Type::ALL_ITERABLE = {
    &Type::ALL,
    &Type::ARMOR,
    &Type::ARMOR_FEET,
    &Type::ARMOR_LEGS,
    &Type::ARMOR_CHEST,
    &Type::ARMOR_HEAD,
    &Type::WEAPON,
    &Type::DIGGER,
    &Type::FISHING_ROD,
    &Type::BREAKABLE,
    &Type::BOW,
    &Type::WEARABLE,
    &Type::TRIDENT
};


EnchantmentRegistry Enchantment::REGISTRY;
lce::CONSOLE Enchantment::currentConsole = lce::CONSOLE::NONE;
LCEVERSION Enchantment::currentVersion = LCEVERSION::NONE;


int Enchantment::getMinCost(c_int enchantmentLevel) {
    return 1 + enchantmentLevel * 10;
}


int Enchantment::getMaxCost(c_int enchantmentLevel) {
    return 6 + enchantmentLevel * 10;
}


bool Enchantment::canApplyTogether(const Enchantment *enchantment) const {
    return this->name != enchantment->name;
}

bool Enchantment::isCompatibleWith(const Enchantment *enchantment) const {
    return this->canApplyTogether(enchantment) && enchantment->canApplyTogether(this);
}


bool Enchantment::canApply(const lce::items::Item *item) const {
    return this->type->canEnchantItem(item);
}


void Enchantment::registerEnchantment(Enchantment* enchantment) {
    REGISTRY.registerValue(enchantment);
}


void Enchantment::registerEnchantments() {
    if (isSetup) {
        #ifdef DEV_MODE
            std::cout << "attempt to register 'Enchantment' class, ignoring" << std::endl;
        #endif
        return;
    }
    isSetup = true;

    registerEnchantment(new EnchantmentProtection("Protection", eRarity::COMMON, 0));
    registerEnchantment(new EnchantmentProtection("Fire Protection", eRarity::UNCOMMON, 1));
    registerEnchantment(new EnchantmentProtection("Feather Falling", eRarity::UNCOMMON, 2));
    registerEnchantment(new EnchantmentProtection("Blast Protection", eRarity::RARE, 3));
    registerEnchantment(new EnchantmentProtection("Projectile Protection", eRarity::UNCOMMON, 4));
    registerEnchantment(new EnchantmentOxygen("Respiration", eRarity::RARE));
    registerEnchantment(new EnchantmentWaterWorker("Aqua Affinity", eRarity::RARE));
    registerEnchantment(new EnchantmentThorns("Thorns", eRarity::EPIC));
    registerEnchantment(new EnchantmentWaterWalker("Depth Strider", eRarity::RARE));
    registerEnchantment(new EnchantmentFrostWalker("Frost Walker", eRarity::RARE));
    registerEnchantment(new EnchantmentBindingCurse("Curse Of Binding", eRarity::EPIC));
    registerEnchantment(new EnchantmentDamage("Sharpness", eRarity::COMMON, 0));
    registerEnchantment(new EnchantmentDamage("Smite", eRarity::UNCOMMON, 1));
    registerEnchantment(new EnchantmentDamage("Bane of Arthropods", eRarity::UNCOMMON, 2));
    registerEnchantment(new EnchantmentKnockback("Knockback", eRarity::UNCOMMON));
    registerEnchantment(new EnchantmentFireAspect("Fire Aspect", eRarity::RARE));
    registerEnchantment(new EnchantmentLootBonus("Looting", eRarity::RARE, &Type::WEAPON));
    registerEnchantment(new EnchantmentDigging("Efficiency", eRarity::COMMON));
    registerEnchantment(new EnchantmentUntouching("Silk Touch", eRarity::EPIC));
    registerEnchantment(new EnchantmentDurability("Unbreaking", eRarity::UNCOMMON));
    registerEnchantment(new EnchantmentLootBonus("Fortune", eRarity::RARE, &Type::DIGGER));
    registerEnchantment(new EnchantmentArrowDamage("Power", eRarity::COMMON));
    registerEnchantment(new EnchantmentArrowKnockback("Punch", eRarity::UNCOMMON));
    registerEnchantment(new EnchantmentArrowFire("Flame", eRarity::RARE));
    registerEnchantment(new EnchantmentArrowInfinite("Infinity", eRarity::EPIC));
    registerEnchantment(new EnchantmentLootBonus("Luck of the Sea", eRarity::RARE, &Type::FISHING_ROD));
    registerEnchantment(new EnchantmentFishingSpeed("Lure", eRarity::RARE));
    registerEnchantment(new EnchantmentMending("Mending", eRarity::RARE));
    registerEnchantment(new EnchantmentVanishingCurse("Curse of Vanishing", eRarity::EPIC));
    registerEnchantment(new EnchantmentTridentImpaler("Impaling", eRarity::UNCOMMON));
    registerEnchantment(new EnchantmentTridentRiptide("Riptide", eRarity::RARE));
    registerEnchantment(new EnchantmentTridentLoyalty("Loyalty", eRarity::COMMON));
    registerEnchantment(new EnchantmentTridentChanneling("Channeling", eRarity::EPIC));
    count = REGISTRY.size();
}
