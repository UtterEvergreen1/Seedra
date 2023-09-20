#pragma once

#include <string>
#include <vector>
#include <utility>
#ifdef INCLUDE_QT
#include <QDebug>
#endif

#include "ItemData.hpp"
#include "LegacyCubiomes/cubiomes/processor.hpp"

namespace Items {
    class Item {
    private:
        std::vector<const Item*> types;
        const int id{};
        const int dataTag{};
        const ItemType type{};
        const std::string name;
        const std::string identifier;
        const bool damageable{};
        const int armorType = EntityEquipmentSlot::NONE;
        const int enchantability = 1;

    public:
        Item() = default;
        Item(int id, int dataTag, ItemType type, std::string itemName, std::string identifier,
             bool damageable, int armorType, int enchantability)
                : id(id), dataTag(dataTag), type(type), name(std::move(itemName)), identifier(std::move(identifier)),
                  damageable(damageable), armorType(armorType), enchantability(enchantability) {}
        Item(int id, int dataTag, ItemType type, std::string itemName, std::string identifier, bool damageable = false)
                : Item(id, dataTag, type, std::move(itemName), std::move(identifier), damageable, EntityEquipmentSlot::NONE, 1) {}
        Item(int id, int dataTag, ItemType type, std::string itemName, std::string identifier, bool damageable, int enchantability)
                : Item(id, dataTag, type, std::move(itemName), std::move(identifier), damageable, EntityEquipmentSlot::NONE, enchantability) {}
        Item(int id, int dataTag, ItemType type, std::string itemName, std::string identifier, int enchantability)
                : Item(id, dataTag, type, std::move(itemName), std::move(identifier), false, EntityEquipmentSlot::NONE, enchantability) {}
        Item(int id, int dataTag, ItemType type, std::string itemName, std::string identifier, int armorType, int enchantability)
                : Item(id, dataTag, type, std::move(itemName), std::move(identifier), false, armorType, enchantability) {}
        Item(int id, int dataTag, std::string itemName, std::string identifier)
                : Item(id, dataTag, ItemType::ItemBlock, std::move(itemName), std::move(identifier), false, EntityEquipmentSlot::NONE, 1) {}


        ND inline int getID() const { return id; }
        ND inline int getDataTag() const { return dataTag; }
        ND inline ItemType getItemType() const { return type; }
        ND inline std::string getName() const { return name; }
        ND inline std::string getIdentifier() const { return identifier; }
        ND inline bool isDamageable() const { return damageable; }
        ND inline int getArmorType() const { return armorType; }
        ND inline int getCost() const { return enchantability; }









        friend std::ostream &operator<<(std::ostream &out, MU const Item &item) {
            out << item.getName();
            return out;
        }

        #ifdef INCLUDE_QT
        friend QDebug operator<<(QDebug out, MU const Item &item) {
                out.nospace() << item.getName();
                return out.nospace();
        }
        #endif
    };

}






