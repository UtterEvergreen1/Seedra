#include <utility>


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
#include "LegacyCubiomes/enchants/classes/EnchantmentSweepingEdge.hpp"
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
const std::vector<std::vector<std::vector<int>>> Enchantment::tableOfOrders = {
    { // XBOX
        { // LATEST
            channeling, riptide, impaling, lure, luck_of_the_sea, punch, fortune,
            silk_touch, looting, depth_strider, unbreaking, bane_of_arthropods, frost_walker, projectile_protection,
            respiration, thorns, loyalty, infinity, power, efficiency, fire_aspect,
            smite, sharpness, protection, feather_falling, aqua_affinity, vanishing_curse, mending,
            flame, binding_curse, fire_protection, blast_protection, knockback
        },
        { // ELYTRA
            lure, luck_of_the_sea, punch, fortune, silk_touch, looting, depth_strider,
            unbreaking, bane_of_arthropods, frost_walker, projectile_protection, respiration, thorns, infinity,
            power, efficiency, fire_aspect, smite, sharpness, protection, feather_falling,
            aqua_affinity, vanishing_curse, mending, flame, binding_curse, fire_protection,
            blast_protection, knockback
        }
    },
    { // PS3
        { // LATEST
            0, 1, 2, 3, 4, 5, 6,
            7, 8, 9, 10, 11, 12, 13,
            14,15, 16, 17, 18, 19, 20,
            21, 22, 23, 24, 25, 26, 27,
            28, 29, 30, 31, 32
        },
        { // ELYTRA
            0, 1, 2, 3, 4, 5, 6,
            7, 8, 9, 10, 11, 12, 13,
            14,15, 16, 17, 18, 19, 20,
            21, 22, 23, 24, 25, 26, 27,
            28
        }
    },
    { // WIIU
        { // LATEST
            channeling, impaling, vanishing_curse, infinity, fire_aspect, flame, punch,
            efficiency, projectile_protection, thorns, looting, knockback, bane_of_arthropods, luck_of_the_sea,
            protection, loyalty, aqua_affinity, sharpness, lure, power, frost_walker,
            fire_protection, respiration, binding_curse, depth_strider, fortune, mending, silk_touch,
            unbreaking, feather_falling, riptide, blast_protection, smite
        },
        { // ELYTRA
            mending, luck_of_the_sea, punch, fortune, unbreaking, efficiency, looting,
            smite, vanishing_curse, binding_curse, silk_touch, frost_walker, flame, depth_strider,
            knockback, bane_of_arthropods, respiration, thorns, sharpness, aqua_affinity, infinity,
            projectile_protection, blast_protection, lure, feather_falling, fire_aspect, fire_protection,
            power, protection
        }
    }
};


bool Enchantment::isSetup = false;
int Enchantment::count = 0;
bool Enchantment::Type::Base::canEnchantItem(const Items::Item *itemIn) const {
    return true;
}


bool Enchantment::Type::All::canEnchantItem(const Items::Item *itemIn) const {
    for (const Type::Base* enumType : ALL_ITERABLE) {
        if (enumType != &ALL && enumType->canEnchantItem(itemIn))
            return true;
    }
    return false;
}


bool      Enchantment::Type::Armor::canEnchantItem(const Items::Item *itemIn) const {return itemIn->getItemType() == Items::ItemType::ItemArmor;}
bool  Enchantment::Type::ArmorFeet::canEnchantItem(const Items::Item *itemIn) const {return itemIn->getArmorType() == Items::EntityEquipmentSlot::FEET;}
bool  Enchantment::Type::ArmorLegs::canEnchantItem(const Items::Item *itemIn) const {return itemIn->getArmorType() == Items::EntityEquipmentSlot::LEGS;}
bool Enchantment::Type::ArmorChest::canEnchantItem(const Items::Item *itemIn) const {return itemIn->getArmorType() == Items::EntityEquipmentSlot::CHEST;}
bool  Enchantment::Type::ArmorHead::canEnchantItem(const Items::Item *itemIn) const {return itemIn->getArmorType() == Items::EntityEquipmentSlot::HEAD;}
bool     Enchantment::Type::Weapon::canEnchantItem(const Items::Item *itemIn) const {return itemIn->getItemType() == Items::ItemType::ItemSword;}
bool     Enchantment::Type::Digger::canEnchantItem(const Items::Item *itemIn) const {return itemIn->getItemType() == Items::ItemType::ItemTool;}
bool Enchantment::Type::FishingRod::canEnchantItem(const Items::Item *itemIn) const {return itemIn->getItemType() == Items::ItemType::ItemFishingRod;}
bool  Enchantment::Type::Breakable::canEnchantItem(const Items::Item *itemIn) const {return itemIn->isDamageable();}
bool        Enchantment::Type::Bow::canEnchantItem(const Items::Item *itemIn) const {return itemIn->getItemType() == Items::ItemType::ItemBow;}
bool    Enchantment::Type::Trident::canEnchantItem(const Items::Item *itemIn) const {return itemIn->getItemType() == Items::ItemType::ItemTrident;}


bool Enchantment::Type::Wearable::canEnchantItem(const Items::Item *itemIn) const {
    return itemIn->getArmorType() != Items::EntityEquipmentSlot::NONE
           || itemIn->getItemType() == Items::ItemSkull
           || itemIn->getItemType() == Items::ItemElytra;
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


RegistryNamespaced<Enchantment> Enchantment::REGISTRY;
CONSOLE Enchantment::currentConsole = CONSOLE::NONE;
LCEVERSION Enchantment::currentVersion = LCEVERSION::NONE;


int Enchantment::getMinCost(int enchantmentLevel) {
    return 1 + enchantmentLevel * 10;
}


int Enchantment::getMaxCost(int enchantmentLevel) {
    return 6 + enchantmentLevel * 10;
}


bool Enchantment::canApplyTogether(const Enchantment *enchantment) const {
    return this->name != enchantment->name;
}


bool Enchantment::canApply(const Items::Item *item) const {
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

    registerEnchantment(new EnchantmentProtection("Protection", &Rarities::COMMON, 0));
    registerEnchantment(new EnchantmentProtection("Fire Protection", &Rarities::UNCOMMON, 1));
    registerEnchantment(new EnchantmentProtection("Feather Falling", &Rarities::UNCOMMON, 2));
    registerEnchantment(new EnchantmentProtection("Blast Protection", &Rarities::RARE, 3));
    registerEnchantment(new EnchantmentProtection("Projectile Protection", &Rarities::UNCOMMON, 4));

    registerEnchantment(new EnchantmentOxygen("Respiration", &Rarities::RARE));
    registerEnchantment(new EnchantmentWaterWorker("Aqua Affinity", &Rarities::RARE));
    registerEnchantment(new EnchantmentThorns("Thorns", &Rarities::EPIC));
    registerEnchantment(new EnchantmentWaterWalker("Depth Strider", &Rarities::RARE));
    registerEnchantment(new EnchantmentFrostWalker("Frost Walker", &Rarities::RARE));
    registerEnchantment(new EnchantmentBindingCurse("Curse Of Binding", &Rarities::EPIC));

    registerEnchantment(new EnchantmentDamage("Sharpness", &Rarities::COMMON, 0));
    registerEnchantment(new EnchantmentDamage("Smite", &Rarities::UNCOMMON, 1));
    registerEnchantment(new EnchantmentDamage("Bane of Arthropods", &Rarities::UNCOMMON, 2));
    registerEnchantment(new EnchantmentKnockback("Knockback", &Rarities::UNCOMMON));
    registerEnchantment(new EnchantmentFireAspect("Fire Aspect", &Rarities::RARE));
    registerEnchantment(new EnchantmentLootBonus("Looting", &Rarities::RARE, &Type::WEAPON));
    registerEnchantment(new EnchantmentDigging("Efficiency", &Rarities::COMMON));
    registerEnchantment(new EnchantmentUntouching("Silk Touch", &Rarities::EPIC));
    registerEnchantment(new EnchantmentDurability("Unbreaking", &Rarities::UNCOMMON));
    registerEnchantment(new EnchantmentLootBonus("fortune", &Rarities::RARE, &Type::DIGGER));
    registerEnchantment(new EnchantmentArrowDamage("Power", &Rarities::COMMON));
    registerEnchantment(new EnchantmentArrowKnockback("Punch", &Rarities::UNCOMMON));
    registerEnchantment(new EnchantmentArrowFire("Flame", &Rarities::RARE));
    registerEnchantment(new EnchantmentArrowInfinite("Infinity", &Rarities::EPIC));
    registerEnchantment(new EnchantmentLootBonus("Luck of the Sea", &Rarities::RARE, &Type::FISHING_ROD));
    registerEnchantment(new EnchantmentFishingSpeed("Lure", &Rarities::RARE));
    registerEnchantment(new EnchantmentMending("Mending", &Rarities::RARE));
    registerEnchantment(new EnchantmentVanishingCurse("Curse of Vanishing", &Rarities::EPIC));
    registerEnchantment(new EnchantmentTridentImpaler("Impaling", &Rarities::UNCOMMON));
    registerEnchantment(new EnchantmentTridentRiptide("Riptide", &Rarities::RARE));
    registerEnchantment(new EnchantmentTridentLoyalty("Loyalty", &Rarities::COMMON));
    registerEnchantment(new EnchantmentTridentChanneling("Channeling", &Rarities::EPIC));
    count = (int)REGISTRY.size();
}
