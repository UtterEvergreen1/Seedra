#include "EnchantWithLevels.hpp"


#include "LegacyCubiomes/enchants/enchantmentHelper.hpp"
#include "LegacyCubiomes/utils/MathHelper.hpp"

/* Initializers */
EnchantWithLevels::EnchantWithLevels(const UniformRoll roll) : randomLevel(roll) {}

EnchantWithLevelsBook::EnchantWithLevelsBook(const UniformRoll levelRange) : EnchantWithLevels(levelRange) {}
EnchantWithLevelsBook::EnchantWithLevelsBook(c_int level) : EnchantWithLevels(UniformRoll(level, level)) {}
EnchantWithLevelsItem::EnchantWithLevelsItem(const UniformRoll levelRange) : EnchantWithLevels(levelRange) {}
EnchantWithLevelsItem::EnchantWithLevelsItem(c_int level) : EnchantWithLevels(UniformRoll(level, level)) {}


/* apply functions */
void EnchantWithLevelsBook::apply(ItemStack& itemStack, RNG& random) {
    c_int level = random.nextInt(this->randomLevel.getMin(), this->randomLevel.getMax());
    ELDataArray* enchantmentVector = buildEnchantmentList(itemStack, random, level);
    enchantmentVector->addEnchantments(itemStack);
}

void EnchantWithLevelsItem::apply(ItemStack& itemStack, RNG& random) {
    c_int level = random.nextInt(this->randomLevel.getMin(), this->randomLevel.getMax());
    addRandomEnchantment(random, itemStack, level);
}


/* other functions */
ELDataArray* EnchantWithLevelsBook::buildEnchantmentList(const ItemStack& itemStackIn, RNG& rng, int level) {

    const lce::items::Item* item = itemStackIn.getItem();
    c_int cost = (item->getCost() >> 2) + 1;

    level += 1 + rng.nextInt(cost) + rng.nextInt(cost);
    const float levelf = static_cast<float>(level);
    c_float f = (rng.nextFloat() + rng.nextFloat() - 1.0F) * 0.15F;
    level = MathHelper::clamp(static_cast<int>(std::round(levelf + levelf * f)), 1, 0x7fffffff);

    ELDataArray* enchants = EnchantmentHelper::BOOK_LEVEL_TABLE.get(level);
    enchants->addRandomItem(rng);

    while (rng.nextInt(50) <= level) {
        const EnchantmentData* back = enchants->getLastEnchantmentAdded();

        for (int enchIndex = 0; enchIndex < enchants->totalEnchants; enchIndex++) {
            if (!back->obj->isCompatibleWith(enchants->data[enchIndex].obj)) {
                // std::cout << enchants->data[enchIndex].obj->name << " removed" << std::endl;
                for (int i = 0; i < enchants->deletions.getIndex(); i++)
                    if (enchIndex == enchants->deletions.getValueAt(i)) goto END;
                enchants->deletions.addItem(enchIndex);
            }
        END:;
        }

        enchants->addRandomItem(rng);
        level /= 2;
    }
    return enchants;
}


EnchDataVec_t EnchantWithLevelsItem::buildEnchantmentList(const ItemStack& itemStackIn, RNG& rng, int level) {
    std::vector<EnchantmentData> list;
    list.reserve(MAX_ENCHANT_LIST_SIZE);

    const lce::items::Item* item = itemStackIn.getItem();
    c_int i = static_cast<unsigned char>(item->getCost());

    if (i == 0) return list;

    level = level + 1 + rng.nextInt(i / 4 + 1) + rng.nextInt(i / 4 + 1);
    const float levelf = static_cast<float>(level);
    const float f = (rng.nextFloat() + rng.nextFloat() - 1.0F) * 0.15F;
    level = MathHelper::clamp(static_cast<int>(std::round(levelf + levelf * f)), 1,
                              std::numeric_limits<int>::max()); // 0x7fffffff

    std::vector<EnchantmentData> list1 = getEnchantmentDataList(level, itemStackIn);

    if (!list1.empty()) {
        EnchantmentData data = WeightedRandom::getRandomItem(rng, list1);
        list.push_back(data);
        while (rng.nextInt(50) <= level) {
            removeIncompatible(list1, list.back());
            if (list1.empty()) break;
            data = WeightedRandom::getRandomItem(rng, list1);
            list.push_back(data);
            level /= 2;
        }
    }
    return list;
}


EnchDataVec_t EnchantWithLevelsItem::getEnchantmentDataList(c_int enchantmentLevelIn, const ItemStack& ItemStackIn) {
    EnchDataVec_t list;

    for (Enchantment* pointer: Enchantment::REGISTRY.getRegistry()) {
        bool added = false;

        if (!pointer->type->canEnchantItem(ItemStackIn.getItem())) { continue; }

        // maxLevel to minLevel - 1 (always 0)
        for (u8 i = pointer->maxLevel; i > 0; --i) {

            if (enchantmentLevelIn >= pointer->getMinCost(i) && enchantmentLevelIn <= pointer->getMaxCost(i)) {
                list.emplace_back(pointer, i);
                added = true;
                break;
            }
        }

        if (!added) {
            // std::cout << "failed to add " << pointer->name << std::endl;
        }
    }

    return list;
}


void EnchantWithLevelsItem::removeIncompatible(EnchDataVec_t& enchDataList, const EnchantmentData enchData) {
    for (auto it = enchDataList.begin(); it != enchDataList.end();) {
        if (!enchData.obj->isCompatibleWith(it->obj)) {
            it = enchDataList.erase(it);
        } else {
            ++it;
        }
    }
}
