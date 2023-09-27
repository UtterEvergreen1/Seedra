#include "enchantmentHelper.hpp"



//==============================================================================
//                           Set Up
//==============================================================================


void EnchantmentHelper::setConsoleAndVersion(CONSOLE console, LCEVERSION version) {
    int consoleNum = static_cast<int>(console);
    int lceVersion = static_cast<int>(version);

    // error handling
    if(consoleNum >= Enchantment::tableOfOrders.size())
        throw std::range_error("Console not implemented.");
    if(lceVersion >= Enchantment::tableOfOrders.at(consoleNum).size())
        throw std::range_error("Version not implemented.");

    if(Enchantment::currentConsole == console && Enchantment::currentVersion == version)
        return;

    Enchantment::currentConsole = console;
    Enchantment::currentVersion = version;
    Enchantment::REGISTRY.orderValues(Enchantment::tableOfOrders[consoleNum][lceVersion]);

    BOOK_LEVEL_TABLE.deallocate();
    BOOK_LEVEL_TABLE.setup();
}


//==============================================================================
//                              Enchant With Levels Items
//==============================================================================


void EnchantmentHelper::EnchantWithLevelsItem::apply(uint64_t *rng, ItemStack *stack, int min, int max) {
    int rand = nextInt(rng, min, max);
    addRandomEnchantment(rng, stack, rand);
}


void EnchantmentHelper::EnchantWithLevelsItem::apply(uint64_t *rng, ItemStack *stack, int level) {
    addRandomEnchantment(rng, stack, level);
}


EnchDataVec_t EnchantmentHelper::EnchantWithLevelsItem::buildEnchantmentList(uint64_t *rng, ItemStack *itemStackIn, int level) {
    std::vector<EnchantmentData> list;
    list.reserve(MAX_ENCHANT_LIST_SIZE);

    const Items::Item* item = itemStackIn->getItem();
    int i = item->getCost();

    if (i == 0)
        return list;

    level = level + 1 + nextInt(rng, i / 4 + 1) + nextInt(rng, i / 4 + 1);
    float f = (nextFloat(rng) + nextFloat(rng) - 1.0F) * 0.15F;
    level = clamp((int)std::round((float)level + (float)level * f), 1, std::numeric_limits<int>::max()); // 0x7fffffff

    std::cout << "Level: " << level << std::endl;

    std::vector<EnchantmentData> list1 = getEnchantmentDataList(level, itemStackIn);

    if (!list1.empty()) {
        EnchantmentData data = WeightedRandom::getRandomItem(rng, list1);
        list.push_back(data);
        while (nextInt(rng, 50) <= level) {
            removeIncompatible(list1, list.back());
            if (list1.empty())
                break;
            data = WeightedRandom::getRandomItem(rng, list1);
            list.push_back(data);
            level /= 2;
        }
    }
    return list;
}


void EnchantmentHelper::EnchantWithLevelsItem::removeIncompatible(EnchDataVec_t& enchDataList, EnchantmentData enchData) {
    std::cout << "REMOVE_INCOMPATIBLE" << std::endl;

    for (auto it = enchDataList.begin(); it != enchDataList.end(); ) {
        if (!enchData.obj->canApplyTogether(it->obj)) {
            std::cout << it->obj->name << " removed" << std::endl;
            it = enchDataList.erase(it);
        } else {
            ++it;
        }
    }
}


inline void EnchantmentHelper::EnchantWithLevelsItem::addRandomEnchantment(uint64_t *const rng, ItemStack *const itemStackIn, int level) {
    EnchDataVec_t enchantmentList = buildEnchantmentList(rng, itemStackIn, level);

    for (const auto &enchantmentData : enchantmentList) {
        itemStackIn->addEnchantment(enchantmentData.obj, enchantmentData.level);
    }
}



EnchDataVec_t EnchantmentHelper::EnchantWithLevelsItem::getEnchantmentDataList(int enchantmentLevelIn, ItemStack *ItemStackIn) {
    EnchDataVec_t list;
    bool added;

    for (Enchantment* pointer : Enchantment::REGISTRY.getRegistry()) {
        added = false;

        if (!pointer->type->canEnchantItem(ItemStackIn->getItem())) continue;

        for (int i = pointer->maxLevel; i > 0; --i) { // maxLevel to minLevel - 1 (always 0)

            if (enchantmentLevelIn >= pointer->getMinCost(i)
                && enchantmentLevelIn <= pointer->getMaxCost(i)) {
                list.emplace_back(pointer, i);
                added = true;
                break;
            }
        }

        if (!added) {
            std::cout << "failed to add " << pointer->name << std::endl;
        }
    }

    return list;
}


//==============================================================================
//                      Enchant With Levels EnchantedBook
//==============================================================================


EnchantedBookEnchantsLookupTable EnchantmentHelper::BOOK_LEVEL_TABLE;


void EnchantmentHelper::EnchantWithLevelsBook::apply(uint64_t *rng, ItemStack *stack, int level) {
    ELDataArray* enchantmentVector = buildEnchantmentList(rng, stack, level);
    enchantmentVector->addEnchantments(stack);
}


ELDataArray* EnchantmentHelper::EnchantWithLevelsBook::buildEnchantmentList(uint64_t *rng, ItemStack *itemStackIn, int level) {

    const Items::Item* item = itemStackIn->getItem();
    int cost = (item->getCost() >> 2) + 1;

    level = level + 1 + nextInt(rng, cost) + nextInt(rng, cost);
    float f = (nextFloat(rng) + nextFloat(rng) - 1.0F) * 0.15F;
    level = clamp((int)std::round((float)level + (float)level * f), 1, 0x7fffffff);

    ELDataArray* enchants = BOOK_LEVEL_TABLE.get(level);
    enchants->addRandomItem(rng);

    while (nextInt(rng, 50) <= level) {
        EnchantmentData* back = enchants->getLastEnchantmentAdded();

        for (int enchIndex = 0; enchIndex < enchants->totalEnchants; enchIndex++) {
            if (!back->obj->canApplyTogether(enchants->data[enchIndex].obj)) {
                // std::cout << enchants->data[enchIndex].obj->name << " removed" << std::endl;
                for (int i = 0; i < enchants->deletions.getIndex(); i++)
                    if (enchIndex == enchants->deletions.getValueAt(i))
                        goto END;
                enchants->deletions.addItem(enchIndex);
            }
            END:;
        }

        enchants->addRandomItem(rng);
        level /= 2;
    }
    return enchants;
}


//==============================================================================
//                              Enchant Randomly Items
//==============================================================================


void EnchantmentHelper::EnchantRandomlyItem::apply(uint64_t *rng, ItemStack *stack) {
    std::vector<Enchantment*> list;
    for (Enchantment* enchantmentPointer : Enchantment::REGISTRY.getRegistry())
        if (enchantmentPointer->canApply(stack->item))
            list.emplace_back(enchantmentPointer);

    Enchantment* enchantmentPointer;
    int rand = nextInt(rng, (int)list.size());
    enchantmentPointer = list[rand];

    int level = nextInt(rng, enchantmentPointer->minLevel, enchantmentPointer->maxLevel);

    stack->addEnchantment(enchantmentPointer, level);
}


//==============================================================================
//                              Enchant Randomly Books
//==============================================================================


void EnchantmentHelper::EnchantRandomlyBook::apply(uint64_t *rng, ItemStack *stack) {
    Enchantment *enchantmentPointer;
    int rand = nextInt(rng, (int)Enchantment::REGISTRY.size());
    enchantmentPointer = Enchantment::REGISTRY[rand];
    int level = nextInt(rng, enchantmentPointer->minLevel, enchantmentPointer->maxLevel);
    stack->addEnchantment(enchantmentPointer, level);
}

