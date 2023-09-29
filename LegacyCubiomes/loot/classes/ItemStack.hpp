#pragma once

#include "LegacyCubiomes/mc/items.hpp"
#include "LegacyCubiomes/cubiomes/rng.hpp"
#include "LegacyCubiomes/enchants/enchantment.hpp"
#include "LegacyCubiomes/enchants/enchantmentData.hpp"

#include "LegacyCubiomes/loot/classes/ItemEntry.hpp"

class ItemStack {
public:
    const Items::Item* item;
    int stackSize{};
    std::vector<EnchantmentData> enchantments;

    ItemStack() : item(nullptr), stackSize(0) {}
    explicit ItemStack(const Items::Item* item) : item(item), stackSize(0) {}
    ItemStack(const Items::Item* item, int stackSize) : item(item), stackSize(stackSize) {}

    ItemStack splitStack(int amount);
    ND const Items::Item* getItem() const;

    void addEnchantment(Enchantment* enchantment, int level);
    void addEnchantmentData(EnchantmentData* enchantmentData);

    friend std::ostream& operator<<(std::ostream& out, const ItemStack &itemStack) {
        if (itemStack.stackSize > 1) {
            out << itemStack.item->getName() << " (" << itemStack.stackSize << ")";
        } else {
            out << itemStack.item->getName();
        }
        if (!itemStack.enchantments.empty()) {
            out << " [";
            for (size_t i = 0; i < itemStack.enchantments.size(); ++i) {
                out << itemStack.enchantments[i].toString();
                if (i < itemStack.enchantments.size() - 1)
                    out << ", ";
            }
            out << "]";
        }
        return out;
    }

#ifdef INCLUDE_QT
    friend QDebug ItemStack::operator<<(QDebug out, const ItemStack &itemStack) {
        if (itemStack.stackSize > 1) {
            out.nospace() << *(itemStack.itemEntry) << " (" << itemStack.stackSize << ")";
        }
        else {
            out.nospace() << *(itemStack.itemEntry);
        }
        return out.nospace();
    }
#endif
};
