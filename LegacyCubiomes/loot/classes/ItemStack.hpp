#pragma once

#include "lce/items/items.hpp"

#include "LegacyCubiomes/enchants/enchantment.hpp"
#include "LegacyCubiomes/enchants/enchantmentData.hpp"


class ItemStack {
public:
    const lce::items::Item* item;
    u8 stackSize{};
    std::vector<EnchantmentData> enchantments;

    ItemStack() : item(nullptr) {}
    explicit ItemStack(const lce::items::Item* item) : item(item) {}
    ItemStack(const lce::items::Item* item, c_int stackSize) : item(item), stackSize(stackSize) {}

    ItemStack splitStack(int amount);
    ND const lce::items::Item* getItem() const;

    void addEnchantment(Enchantment* enchantment, int level);
    void addEnchantmentData(EnchantmentData* enchantmentData);

    friend std::ostream& operator<<(std::ostream& out, const ItemStack& itemStack) {
        if (itemStack.stackSize > 1) {
            out << itemStack.item->getName() << " (" << static_cast<int>(itemStack.stackSize) << ")";
        } else {
            out << itemStack.item->getName();
        }
        if (!itemStack.enchantments.empty()) {
            out << " [";
            for (size_t i = 0; i < itemStack.enchantments.size(); ++i) {
                out << itemStack.enchantments[i].toString();
                if (i < itemStack.enchantments.size() - 1) out << ", ";
            }
            out << "]";
        }
        return out;
    }

#ifdef INCLUDE_QT
    friend QDebug operator<<(QDebug out, const ItemStack& itemStack) {
        if (itemStack.stackSize > 1) {
            out.nospace() << QString::fromStdString(itemStack.getItem()->getName()) << " (" << itemStack.stackSize
                          << ")";
        } else {
            out.nospace() << QString::fromStdString(itemStack.getItem()->getName());
        }
        return out.nospace();
    }
#endif
};
