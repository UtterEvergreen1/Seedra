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
        const std::string name;
        const std::string identifier;
        const uint16_t id{};
        const uint8_t dataTag{};
        const EntityEquipSlot armorType = EntityEquipSlot::NONE;
        const int8_t enchantability = 1;
        const ItemType type{};
        const bool damageable{};


    public:
        Item() = default;
        Item(uint16_t id, uint8_t dataTag, ItemType type, std::string itemName, std::string identifier,
             bool damageable, EntityEquipSlot armorType, int8_t enchantability)
                : id(id), dataTag(dataTag), type(type), name(std::move(itemName)), identifier(std::move(identifier)),
                  damageable(damageable), armorType(armorType), enchantability(enchantability) {}
        Item(uint16_t id, uint8_t dataTag, ItemType type, std::string itemName, std::string identifier, bool damageable = false)
                : Item(id, dataTag, type, std::move(itemName), std::move(identifier), damageable, EntityEquipSlot::NONE, 1) {}
        Item(uint16_t id, uint8_t dataTag, ItemType type, std::string itemName, std::string identifier, bool damageable, int8_t enchantability)
                : Item(id, dataTag, type, std::move(itemName), std::move(identifier), damageable, EntityEquipSlot::NONE, enchantability) {}
        Item(uint16_t id, uint8_t dataTag, ItemType type, std::string itemName, std::string identifier, int8_t enchantability)
                : Item(id, dataTag, type, std::move(itemName), std::move(identifier), false, EntityEquipSlot::NONE, enchantability) {}
        Item(uint16_t id, uint8_t dataTag, ItemType type, std::string itemName, std::string identifier, EntityEquipSlot armorType, int8_t enchantability)
                : Item(id, dataTag, type, std::move(itemName), std::move(identifier), false, armorType, enchantability) {}
        Item(uint16_t id, uint8_t dataTag, std::string itemName, std::string identifier)
                : Item(id, dataTag, ItemType::ItemBlock, std::move(itemName), std::move(identifier), false, EntityEquipSlot::NONE, 1) {}


        ND inline uint16_t getID() const { return id; }
        ND inline uint8_t getDataTag() const { return dataTag; }
        ND inline ItemType getItemType() const { return type; }
        ND inline std::string getName() const { return name; }
        ND inline std::string getIdentifier() const { return "minecraft:" + identifier; }
        ND inline bool isDamageable() const { return damageable; }
        ND inline EntityEquipSlot getArmorType() const { return armorType; }
        ND inline int8_t getCost() const { return enchantability; }

        friend std::ostream &operator<<(std::ostream &out, const Item &item) {
            out << item.getName();
            return out;
        }

        #ifdef INCLUDE_QT
        friend QDebug operator<<(QDebug out, const Item &item) {
                out.nospace() << QString::fromStdString(item.getName());
                return out.nospace();
        }
        #endif
    };

}






