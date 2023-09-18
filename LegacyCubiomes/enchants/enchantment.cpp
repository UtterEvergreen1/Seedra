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

bool      Enchantment::Type::Armor::canEnchantItem(const Items::Item *itemIn) const {return itemIn->type == Items::ItemType::ItemArmor;}
bool  Enchantment::Type::ArmorFeet::canEnchantItem(const Items::Item *itemIn) const {return itemIn->armorType == Items::EntityEquipmentSlot::FEET;}
bool  Enchantment::Type::ArmorLegs::canEnchantItem(const Items::Item *itemIn) const {return itemIn->armorType == Items::EntityEquipmentSlot::LEGS;}
bool Enchantment::Type::ArmorChest::canEnchantItem(const Items::Item *itemIn) const {return itemIn->armorType == Items::EntityEquipmentSlot::CHEST;}
bool  Enchantment::Type::ArmorHead::canEnchantItem(const Items::Item *itemIn) const {return itemIn->armorType == Items::EntityEquipmentSlot::HEAD;}
bool     Enchantment::Type::Weapon::canEnchantItem(const Items::Item *itemIn) const {return itemIn->type == Items::ItemType::ItemSword;}
bool     Enchantment::Type::Digger::canEnchantItem(const Items::Item *itemIn) const {return itemIn->type == Items::ItemType::ItemTool;}
bool Enchantment::Type::FishingRod::canEnchantItem(const Items::Item *itemIn) const {return itemIn->type == Items::ItemType::ItemFishingRod;}
bool  Enchantment::Type::Breakable::canEnchantItem(const Items::Item *itemIn) const {return itemIn->isDamageable();}
bool        Enchantment::Type::Bow::canEnchantItem(const Items::Item *itemIn) const {return itemIn->type == Items::ItemType::ItemBow;}

bool Enchantment::Type::Wearable::canEnchantItem(const Items::Item *itemIn) const {
    return itemIn->armorType != Items::EntityEquipmentSlot::NONE
           || itemIn->type == Items::ItemSkull
           || itemIn->type == Items::ItemElytra;
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
std::vector<const Enchantment::Type::Base*> Enchantment::Type::ALL_ITERABLE = {};


std::map<int, Enchantment*> Enchantment::REGISTRY = {};

int Enchantment::getMinEnchantability(int enchantmentLevel) {
    return 1 + enchantmentLevel * 10;
}
int Enchantment::getMaxEnchantability(int enchantmentLevel) {
    return this->getMinEnchantability(enchantmentLevel) + 5;
}

bool Enchantment::canApplyTogether(const Enchantment *enchantment) const {
    return this->name != enchantment->name;
}

bool Enchantment::canApply(const Items::Item *item) const {
    return this->type->canEnchantItem(item);
}

void Enchantment::registerEnchantment(int id, Enchantment* enchantment) {
    REGISTRY.emplace(id, enchantment);
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
            &Type::WEARABLE
    };

}


void Enchantment::registerEnchantments() {
    initializeTypeIterable();

    #ifdef IS_WIIU
    registerEnchantment(0, new EnchantmentMending(         "Mending",              &Rarities::RARE));
    registerEnchantment(1, new EnchantmentLootBonus(       "Luck of the Sea",      &Rarities::RARE, &Type::FISHING_ROD));
    registerEnchantment(2, new EnchantmentArrowKnockback(  "Punch",                &Rarities::UNCOMMON));
    registerEnchantment(3, new EnchantmentLootBonus(       "fortune",              &Rarities::RARE, &Type::DIGGER));
    registerEnchantment(4, new EnchantmentDurability(      "Unbreaking",           &Rarities::UNCOMMON));
    registerEnchantment(5, new EnchantmentDigging(         "Efficiency",           &Rarities::COMMON));
    registerEnchantment(6, new EnchantmentLootBonus(       "Looting",              &Rarities::RARE, &Type::WEAPON));
    registerEnchantment(7, new EnchantmentDamage(          "Smite",                &Rarities::UNCOMMON, 1));
    registerEnchantment(8, new EnchantmentVanishingCurse(  "Curse of Vanishing",   &Rarities::EPIC));
    registerEnchantment(9, new EnchantmentBindingCurse(    "Curse Of Binding",     &Rarities::EPIC));
    registerEnchantment(10, new EnchantmentUntouching(      "Silk Touch",           &Rarities::EPIC));
    registerEnchantment(11, new EnchantmentFrostWalker(      "Frost Walker",         &Rarities::RARE));
    registerEnchantment(12, new EnchantmentArrowFire(       "Flame",                &Rarities::RARE));
    registerEnchantment(13, new EnchantmentWaterWalker(      "Depth Strider",        &Rarities::RARE));
    registerEnchantment(14, new EnchantmentKnockback(       "Knockback",            &Rarities::UNCOMMON));
    registerEnchantment(15, new EnchantmentDamage(          "Bane of Arthropods",   &Rarities::UNCOMMON, 2));
    registerEnchantment(16, new EnchantmentOxygen(           "Respiration",          &Rarities::RARE));
    registerEnchantment(17, new EnchantmentThorns(           "Thorns",               &Rarities::EPIC));
    registerEnchantment(18, new EnchantmentDamage(          "Sharpness",            &Rarities::COMMON, 0));
    registerEnchantment(19, new EnchantmentWaterWorker(      "Aqua Affinity",        &Rarities::RARE));
    registerEnchantment(20, new EnchantmentArrowInfinite(   "Infinity",             &Rarities::EPIC));
    registerEnchantment(21, new EnchantmentProtection(       "Projectile Protection",&Rarities::UNCOMMON, 4));
    registerEnchantment(22, new EnchantmentProtection(       "Blast Protection",     &Rarities::RARE, 3));
    registerEnchantment(23, new EnchantmentFishingSpeed(    "Lure",                 &Rarities::RARE));
    registerEnchantment(24, new EnchantmentProtection(       "Feather Falling",      &Rarities::UNCOMMON, 2));
    registerEnchantment(25, new EnchantmentFireAspect(      "Fire Aspect",          &Rarities::RARE));
    registerEnchantment(26, new EnchantmentProtection(       "Fire Protection",      &Rarities::UNCOMMON, 1));
    registerEnchantment(27, new EnchantmentArrowDamage(     "Power",                &Rarities::COMMON));
    registerEnchantment(28, new EnchantmentProtection(       "Protection",           &Rarities::COMMON, 0));
    // 80 impaling      UNCOMMON
    // 81 riptide       RARE
    // 82 loyalty       COMMON
    // 83 channeling    VERY_RARE
    #else
    registerEnchantment(0, new EnchantmentProtection(       "Protection",           &Rarities::COMMON, 0));
    registerEnchantment(1, new EnchantmentProtection(       "Fire Protection",      &Rarities::UNCOMMON, 1));
    registerEnchantment(2, new EnchantmentProtection(       "Feather Falling",      &Rarities::UNCOMMON, 2));
    registerEnchantment(3, new EnchantmentProtection(       "Blast Protection",     &Rarities::RARE, 3));
    registerEnchantment(4, new EnchantmentProtection(       "Projectile Protection",&Rarities::UNCOMMON, 4));

    registerEnchantment(5, new EnchantmentOxygen(           "Respiration",          &Rarities::RARE));
    registerEnchantment(6, new EnchantmentWaterWorker(      "Aqua Affinity",        &Rarities::RARE));
    registerEnchantment(7, new EnchantmentThorns(           "Thorns",               &Rarities::EPIC));
    registerEnchantment(8, new EnchantmentWaterWalker(      "Depth Strider",        &Rarities::RARE));
    registerEnchantment(9, new EnchantmentFrostWalker(      "Frost Walker",         &Rarities::RARE));
    registerEnchantment(10, new EnchantmentBindingCurse(    "Curse Of Binding",     &Rarities::EPIC));

    registerEnchantment(16, new EnchantmentDamage(          "Sharpness",            &Rarities::COMMON, 0));
    registerEnchantment(17, new EnchantmentDamage(          "Smite",                &Rarities::UNCOMMON, 1));
    registerEnchantment(18, new EnchantmentDamage(          "Bane of Arthropods",   &Rarities::UNCOMMON, 2));
    registerEnchantment(19, new EnchantmentKnockback(       "Knockback",            &Rarities::UNCOMMON));
    registerEnchantment(20, new EnchantmentFireAspect(      "Fire Aspect",          &Rarities::RARE));
    registerEnchantment(21, new EnchantmentLootBonus(       "Looting",              &Rarities::RARE, &Type::WEAPON));
    #ifdef INCLUDE_JAVA
    registerEnchantment(22, new EnchantmentSweepingEdge(    "sweeping",             &Rarities::RARE));
    #endif
    registerEnchantment(32, new EnchantmentDigging(         "Efficiency",           &Rarities::COMMON));
    registerEnchantment(33, new EnchantmentUntouching(      "Silk Touch",           &Rarities::EPIC));
    registerEnchantment(34, new EnchantmentDurability(      "Unbreaking",           &Rarities::UNCOMMON));
    registerEnchantment(35, new EnchantmentLootBonus(       "fortune",              &Rarities::RARE, &Type::DIGGER));
    registerEnchantment(48, new EnchantmentArrowDamage(     "Power",                &Rarities::COMMON));
    registerEnchantment(49, new EnchantmentArrowKnockback(  "Punch",                &Rarities::UNCOMMON));
    registerEnchantment(50, new EnchantmentArrowFire(       "Flame",                &Rarities::RARE));
    registerEnchantment(51, new EnchantmentArrowInfinite(   "Infinity",             &Rarities::EPIC));
    registerEnchantment(61, new EnchantmentLootBonus(       "Luck of the Sea",      &Rarities::RARE, &Type::FISHING_ROD));
    registerEnchantment(62, new EnchantmentFishingSpeed(    "Lure",                 &Rarities::RARE));
    registerEnchantment(70, new EnchantmentMending(         "Mending",              &Rarities::RARE));
    registerEnchantment(71, new EnchantmentVanishingCurse(  "Curse of Vanishing",   &Rarities::EPIC));
    // 80 impaling      UNCOMMON
    // 81 riptide       RARE
    // 82 loyalty       COMMON
    // 83 channeling    VERY_RARE
    #endif

}

std::string EnchantmentData::LEVEL_ROMAN[6] = {"", "", "II", "III", "IV", "V"};



std::string EnchantmentData::toString() const {
    if (obj == nullptr)
        return "NULLPTR";
    if (level < 2)
        return obj->name;
    return obj->name + " " + LEVEL_ROMAN[level];
}












