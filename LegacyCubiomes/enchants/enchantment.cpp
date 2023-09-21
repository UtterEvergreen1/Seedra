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





bool Enchantment::isSetup = false;


Enchantment::~Enchantment() {
    for (auto &pair : REGISTRY) {
        delete pair.second;
    }
    REGISTRY.clear();
}

bool Enchantment::Type::Base::canEnchantItem(const Items::Item *itemIn) const {
    return true;
}

bool Enchantment::Type::All::canEnchantItem(const Items::Item *itemIn) const {
    for (auto enumType : ALL_ITERABLE) {
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

std::vector<const Enchantment::Type::Base*> Enchantment::Type::ALL_ITERABLE = {};


std::map<int, Enchantment*> Enchantment::REGISTRY = {};

int Enchantment::getMinCost(int enchantmentLevel) {
    return 1 + enchantmentLevel * 10;
}
int Enchantment::getMaxCost(int enchantmentLevel) {
    return 6+ enchantmentLevel * 10;
}

bool Enchantment::canApplyTogether(const Enchantment *enchantment) const {
    return this->name != enchantment->name;
}

bool Enchantment::canApply(const Items::Item *item) const {
    return this->type->canEnchantItem(item);
}

void Enchantment::initializeTypeIterable() {
    Type::ALL_ITERABLE = {
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
}

void Enchantment::registerEnch(int *id, Enchantment* enchantment) {
    REGISTRY.emplace(*id, enchantment);
    (*id)++;
}


template<bool isAquatic>
void Enchantment::registerEnchantments() {
    if (isSetup) {
        #ifdef DEBUG
        std::cout << "attempt to register 'Enchantment' class, ignoring" << std::endl;
        #endif
        return;
    }
    isSetup = true;

    initializeTypeIterable();

    int id = 0;


    #ifdef IS_WIIU
    if constexpr (isAquatic) { // WIIU, Aquatic
        std::cout << "WIIU AQUATIC" << std::endl;
        registerEnch(&id, new EnchantmentTridentChanneling("Channeling", &Rarities::EPIC));
        registerEnch(&id, new EnchantmentTridentImpaler("Impaling", &Rarities::UNCOMMON));
        registerEnch(&id, new EnchantmentVanishingCurse("Curse of Vanishing", &Rarities::EPIC));
        registerEnch(&id, new EnchantmentArrowInfinite("Infinity", &Rarities::EPIC));
        registerEnch(&id, new EnchantmentFireAspect("Fire Aspect", &Rarities::RARE));
        registerEnch(&id, new EnchantmentArrowFire("Flame", &Rarities::RARE));
        registerEnch(&id, new EnchantmentArrowKnockback("Punch", &Rarities::UNCOMMON));
        registerEnch(&id, new EnchantmentDigging("Efficiency", &Rarities::COMMON));
        registerEnch(&id, new EnchantmentProtection("Projectile Protection", &Rarities::UNCOMMON, 4));
        registerEnch(&id, new EnchantmentThorns("Thorns", &Rarities::EPIC));
        registerEnch(&id, new EnchantmentLootBonus("Looting", &Rarities::RARE, &Type::WEAPON));
        registerEnch(&id, new EnchantmentKnockback("Knockback", &Rarities::UNCOMMON));
        registerEnch(&id, new EnchantmentDamage("Bane of Arthropods", &Rarities::UNCOMMON, 2));
        registerEnch(&id, new EnchantmentLootBonus("Luck of the Sea", &Rarities::RARE, &Type::FISHING_ROD));
        registerEnch(&id, new EnchantmentProtection("Protection", &Rarities::COMMON, 0));
        registerEnch(&id, new EnchantmentTridentLoyalty("Loyalty", &Rarities::COMMON));
        registerEnch(&id, new EnchantmentWaterWorker("Aqua Affinity", &Rarities::RARE));
        registerEnch(&id, new EnchantmentDamage("Sharpness", &Rarities::COMMON, 0));
        registerEnch(&id, new EnchantmentFishingSpeed("Lure", &Rarities::RARE));
        registerEnch(&id, new EnchantmentArrowDamage("Power", &Rarities::COMMON));
        registerEnch(&id, new EnchantmentFrostWalker("Frost Walker", &Rarities::RARE));
        registerEnch(&id, new EnchantmentProtection("Fire Protection", &Rarities::UNCOMMON, 1));
        registerEnch(&id, new EnchantmentOxygen("Respiration", &Rarities::RARE));
        registerEnch(&id, new EnchantmentBindingCurse("Curse Of Binding", &Rarities::EPIC));
        registerEnch(&id, new EnchantmentWaterWalker("Depth Strider", &Rarities::RARE));
        registerEnch(&id, new EnchantmentLootBonus("fortune", &Rarities::RARE, &Type::DIGGER));
        registerEnch(&id, new EnchantmentMending("Mending", &Rarities::RARE));
        registerEnch(&id, new EnchantmentUntouching("Silk Touch", &Rarities::EPIC));
        registerEnch(&id, new EnchantmentDurability("Unbreaking", &Rarities::UNCOMMON));
        registerEnch(&id, new EnchantmentProtection("Feather Falling", &Rarities::UNCOMMON, 2));
        registerEnch(&id, new EnchantmentTridentRiptide("Riptide", &Rarities::RARE));
        registerEnch(&id, new EnchantmentProtection("Blast Protection", &Rarities::RARE, 3));
        registerEnch(&id, new EnchantmentDamage("Smite", &Rarities::UNCOMMON, 1));



    } else { // WIIU, Elytra Update
        std::cout << "WIIU ELYTRA" << std::endl;
        registerEnch(&id, new EnchantmentMending("Mending", &Rarities::RARE));
        registerEnch(&id, new EnchantmentLootBonus("Luck of the Sea", &Rarities::RARE, &Type::FISHING_ROD));
        registerEnch(&id, new EnchantmentArrowKnockback("Punch", &Rarities::UNCOMMON));
        registerEnch(&id, new EnchantmentLootBonus("fortune", &Rarities::RARE, &Type::DIGGER));
        registerEnch(&id, new EnchantmentDurability("Unbreaking", &Rarities::UNCOMMON));
        registerEnch(&id, new EnchantmentDigging("Efficiency", &Rarities::COMMON));
        registerEnch(&id, new EnchantmentLootBonus("Looting", &Rarities::RARE, &Type::WEAPON));
        registerEnch(&id, new EnchantmentDamage("Smite", &Rarities::UNCOMMON, 1));
        registerEnch(&id, new EnchantmentVanishingCurse("Curse of Vanishing", &Rarities::EPIC));
        registerEnch(&id, new EnchantmentBindingCurse("Curse Of Binding", &Rarities::EPIC));
        registerEnch(&id, new EnchantmentUntouching("Silk Touch", &Rarities::EPIC));
        registerEnch(&id, new EnchantmentFrostWalker("Frost Walker", &Rarities::RARE));
        registerEnch(&id, new EnchantmentArrowFire("Flame", &Rarities::RARE));
        registerEnch(&id, new EnchantmentWaterWalker("Depth Strider", &Rarities::RARE));
        registerEnch(&id, new EnchantmentKnockback("Knockback", &Rarities::UNCOMMON));
        registerEnch(&id, new EnchantmentDamage("Bane of Arthropods", &Rarities::UNCOMMON, 2));
        registerEnch(&id, new EnchantmentOxygen("Respiration", &Rarities::RARE));
        registerEnch(&id, new EnchantmentThorns("Thorns", &Rarities::EPIC));
        registerEnch(&id, new EnchantmentDamage("Sharpness", &Rarities::COMMON, 0));
        registerEnch(&id, new EnchantmentWaterWorker("Aqua Affinity", &Rarities::RARE));
        registerEnch(&id, new EnchantmentArrowInfinite("Infinity", &Rarities::EPIC));
        registerEnch(&id, new EnchantmentProtection("Projectile Protection", &Rarities::UNCOMMON, 4));
        registerEnch(&id, new EnchantmentProtection("Blast Protection", &Rarities::RARE, 3));
        registerEnch(&id, new EnchantmentFishingSpeed("Lure", &Rarities::RARE));
        registerEnch(&id, new EnchantmentProtection("Feather Falling", &Rarities::UNCOMMON, 2));
        registerEnch(&id, new EnchantmentFireAspect("Fire Aspect", &Rarities::RARE));
        registerEnch(&id, new EnchantmentProtection("Fire Protection", &Rarities::UNCOMMON, 1));
        registerEnch(&id, new EnchantmentArrowDamage("Power", &Rarities::COMMON));
        registerEnch(&id, new EnchantmentProtection("Protection", &Rarities::COMMON, 0));
    }
    #else
    registerEnchantment(&id, new EnchantmentProtection(       "Protection",           &Rarities::COMMON, 0));
    registerEnchantment(&id, new EnchantmentProtection(       "Fire Protection",      &Rarities::UNCOMMON, 1));
    registerEnchantment(&id, new EnchantmentProtection(       "Feather Falling",      &Rarities::UNCOMMON, 2));
    registerEnchantment(&id, new EnchantmentProtection(       "Blast Protection",     &Rarities::RARE, 3));
    registerEnchantment(&id, new EnchantmentProtection(       "Projectile Protection",&Rarities::UNCOMMON, 4));
    registerEnchantment(&id, new EnchantmentOxygen(           "Respiration",          &Rarities::RARE));
    registerEnchantment(&id, new EnchantmentWaterWorker(      "Aqua Affinity",        &Rarities::RARE));
    registerEnchantment(&id, new EnchantmentThorns(           "Thorns",               &Rarities::EPIC));
    registerEnchantment(&id, new EnchantmentWaterWalker(      "Depth Strider",        &Rarities::RARE));
    registerEnchantment(&id, new EnchantmentFrostWalker(      "Frost Walker",         &Rarities::RARE));
    registerEnchantment(&id, new EnchantmentBindingCurse(    "Curse Of Binding",     &Rarities::EPIC));
    registerEnchantment(&id, new EnchantmentDamage(          "Sharpness",            &Rarities::COMMON, 0));
    registerEnchantment(&id, new EnchantmentDamage(          "Smite",                &Rarities::UNCOMMON, 1));
    registerEnchantment(&id, new EnchantmentDamage(          "Bane of Arthropods",   &Rarities::UNCOMMON, 2));
    registerEnchantment(&id, new EnchantmentKnockback(       "Knockback",            &Rarities::UNCOMMON));
    registerEnchantment(&id, new EnchantmentFireAspect(      "Fire Aspect",          &Rarities::RARE));
    registerEnchantment(&id, new EnchantmentLootBonus(       "Looting",              &Rarities::RARE, &Type::WEAPON));
    #ifdef INCLUDE_JAVA
    registerEnchantment(&id, new EnchantmentSweepingEdge(    "sweeping",             &Rarities::RARE));
    #endif
    registerEnchantment(&id, new EnchantmentDigging(         "Efficiency",           &Rarities::COMMON));
    registerEnchantment(&id, new EnchantmentUntouching(      "Silk Touch",           &Rarities::EPIC));
    registerEnchantment(&id, new EnchantmentDurability(      "Unbreaking",           &Rarities::UNCOMMON));
    registerEnchantment(&id, new EnchantmentLootBonus(       "fortune",              &Rarities::RARE, &Type::DIGGER));
    registerEnchantment(&id, new EnchantmentArrowDamage(     "Power",                &Rarities::COMMON));
    registerEnchantment(&id, new EnchantmentArrowKnockback(  "Punch",                &Rarities::UNCOMMON));
    registerEnchantment(&id, new EnchantmentArrowFire(       "Flame",                &Rarities::RARE));
    registerEnchantment(&id, new EnchantmentArrowInfinite(   "Infinity",             &Rarities::EPIC));
    registerEnchantment(&id, new EnchantmentLootBonus(       "Luck of the Sea",      &Rarities::RARE, &Type::FISHING_ROD));
    registerEnchantment(&id, new EnchantmentFishingSpeed(    "Lure",                 &Rarities::RARE));
    registerEnchantment(&id, new EnchantmentMending(         "Mending",              &Rarities::RARE));
    registerEnchantment(&id, new EnchantmentVanishingCurse(  "Curse of Vanishing",   &Rarities::EPIC));

    // these ids are made up idk what the actual enchantment ids are
    registerEnchantment(&id, new EnchantmentTridentImpaler("Impaling",           &Rarities::UNCOMMON));
    registerEnchantment(&id, new EnchantmentTridentRiptide("Riptide",            &Rarities::RARE));
    registerEnchantment(&id, new EnchantmentTridentLoyalty("Loyalty",              &Rarities::COMMON));
    registerEnchantment(&id, new EnchantmentTridentChanneling("Channeling",       &Rarities::EPIC));

    // 80 impaling      UNCOMMON
    // 81 riptide       RARE
    // 82 loyalty       COMMON
    // 83 channeling    VERY_RARE
    #endif

    count = (int)REGISTRY.size();

}



int Enchantment::count = 0;
template void Enchantment::registerEnchantments<true>();
template void Enchantment::registerEnchantments<false>();









