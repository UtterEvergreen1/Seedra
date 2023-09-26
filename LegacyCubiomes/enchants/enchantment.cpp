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

///TODO: FINISH TABLE
const std::vector<std::vector<std::vector<int>>> Enchantment::tableOfPointers = {
        { // XBOX
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
                        0x1827fb28, 0x1827fb00, 0x1827fe58, 0x18280030,
                        0x1827fa60, 0x18280598, 0x182805c0, 0x1827ffe0,
                        0x182801b8, 0x182809e8, 0x18280c38, 0x18280c60,
                        0x18280760, 0x18281018, 0x182813e8, 0x18281288,
                        0x182813c0, 0x18281a00, 0x18281528, 0x18281988,
                        0x18281a28, 0x1827fdb8, 0x18281e68, 0x18281ad0,
                        0x18282458, 0x182826a8, 0x182822e0, 0x18282830,
                        0x18282108, 0x18282da0, 0x18281ab0, 0x18282f28,
                        0x182834d8
                },
                { // ELYTRA
                        0, 1, 2, 3, 4, 5, 6,
                        7, 8, 9, 10, 11, 12, 13,
                        14,15, 16, 17, 18, 19, 20,
                        21, 22, 23, 24, 25, 26, 27,
                        28
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
CONSOLE Enchantment::currentConsole;
LCEVERSION Enchantment::currentVersion;

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

void Enchantment::registerEnchantment(int id, std::string name, Enchantment* enchantment, int fakePointer) {
    ResourceLocation resourceLocation(name);
    REGISTRY.registerMapping(id, resourceLocation, enchantment, fakePointer);
}

void Enchantment::setConsoleAndVersion(CONSOLE console, LCEVERSION version){
    if(Enchantment::currentConsole == console && Enchantment::currentVersion == version)
        return;

    Enchantment::currentConsole = console;
    Enchantment::currentVersion = version;
    Enchantment::REGISTRY.reorderMapping(Enchantment::tableOfPointers[static_cast<int>(console)][static_cast<int>(version)]);
}

void Enchantment::registerEnchantments() {
    if (isSetup) {
#ifdef DEV_MODE
        std::cout << "attempt to register 'Enchantment' class, ignoring" << std::endl;
#endif
        return;
    }
    isSetup = true;

    registerEnchantment(0, "protection", new EnchantmentProtection("Protection", &Rarities::COMMON, 0), 0x1827fb28);
    registerEnchantment(1, "fire_protection", new EnchantmentProtection("Fire Protection", &Rarities::UNCOMMON, 1), 0x1827fb00);
    registerEnchantment(2, "feather_falling", new EnchantmentProtection("Feather Falling", &Rarities::UNCOMMON, 2), 0x1827fe58);
    registerEnchantment(3, "blast_protection", new EnchantmentProtection("Blast Protection", &Rarities::RARE, 3), 0x18280030);
    registerEnchantment(4, "projectile_protection", new EnchantmentProtection("Projectile Protection", &Rarities::UNCOMMON, 4), 0x1827fa60);

    registerEnchantment(5, "respiration", new EnchantmentOxygen("Respiration", &Rarities::RARE), 0x18280598);
    registerEnchantment(6, "aqua_affinity", new EnchantmentWaterWorker("Aqua Affinity", &Rarities::RARE), 0x182805c0);
    registerEnchantment(7, "thorns", new EnchantmentThorns("Thorns", &Rarities::EPIC), 0x1827ffe0);
    registerEnchantment(8, "depth_strider", new EnchantmentWaterWalker("Depth Strider", &Rarities::RARE), 0x182801b8);
    registerEnchantment(9, "frost_walker", new EnchantmentFrostWalker("Frost Walker", &Rarities::RARE), 0x182809e8);
    registerEnchantment(10, "binding_curse", new EnchantmentBindingCurse("Curse Of Binding", &Rarities::EPIC), 0x18280c38);

    registerEnchantment(16, "sharpness", new EnchantmentDamage("Sharpness", &Rarities::COMMON, 0), 0x18280c60);
    registerEnchantment(17, "smite", new EnchantmentDamage("Smite", &Rarities::UNCOMMON, 1), 0x18280760);
    registerEnchantment(18, "bane_of_arthropods", new EnchantmentDamage("Bane of Arthropods", &Rarities::UNCOMMON, 2), 0x18281018);
    registerEnchantment(19, "knockback", new EnchantmentKnockback("Knockback", &Rarities::UNCOMMON), 0x182813e8);
    registerEnchantment(20, "fire_aspect", new EnchantmentFireAspect("Fire Aspect", &Rarities::RARE), 0x18281288);
    registerEnchantment(21, "looting", new EnchantmentLootBonus("Looting", &Rarities::RARE, &Type::WEAPON), 0x182813c0);
    registerEnchantment(32, "efficiency", new EnchantmentDigging("Efficiency", &Rarities::COMMON), 0x18281a00);
    registerEnchantment(33, "silk_touch", new EnchantmentUntouching("Silk Touch", &Rarities::EPIC), 0x18281528);
    registerEnchantment(34, "unbreaking", new EnchantmentDurability("Unbreaking", &Rarities::UNCOMMON), 0x18281988);
    registerEnchantment(35, "fortune", new EnchantmentLootBonus("fortune", &Rarities::RARE, &Type::DIGGER), 0x18281a28);
    registerEnchantment(48, "power", new EnchantmentArrowDamage("Power", &Rarities::COMMON), 0x1827fdb8);
    registerEnchantment(49, "punch", new EnchantmentArrowKnockback("Punch", &Rarities::UNCOMMON), 0x18281e68);
    registerEnchantment(50, "flame", new EnchantmentArrowFire("Flame", &Rarities::RARE), 0x18281ad0);
    registerEnchantment(51, "infinity", new EnchantmentArrowInfinite("Infinity", &Rarities::EPIC), 0x18282458);
    registerEnchantment(61, "luck_of_the_sea", new EnchantmentLootBonus("Luck of the Sea", &Rarities::RARE, &Type::FISHING_ROD), 0x182826a8);
    registerEnchantment(62, "lure", new EnchantmentFishingSpeed("Lure", &Rarities::RARE), 0x182822e0);
    registerEnchantment(70, "mending", new EnchantmentMending("Mending", &Rarities::RARE), 0x18282830);
    registerEnchantment(71, "vanishing_curse", new EnchantmentVanishingCurse("Curse of Vanishing", &Rarities::EPIC), 0x18282108);
    registerEnchantment(80, "impaling", new EnchantmentTridentImpaler("Impaling", &Rarities::UNCOMMON), 0x18282da0);
    registerEnchantment(81, "riptide", new EnchantmentTridentRiptide("Riptide", &Rarities::RARE), 0x18281ab0);
    registerEnchantment(82, "loyalty", new EnchantmentTridentLoyalty("Loyalty", &Rarities::COMMON), 0x18282f28);
    registerEnchantment(83, "channeling", new EnchantmentTridentChanneling("Channeling", &Rarities::EPIC), 0x182834d8);
    REGISTRY.orderMapping();
    count = (int)REGISTRY.size();
}

