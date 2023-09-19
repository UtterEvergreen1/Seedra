#include "enchantmentHelperBook.hpp"


EnchantedBookEnchantsLookupTable EnchantmentHelperBook::BOOK_LEVEL_TABLE;


//==============================================================================
//                              Enchant With Levels
//==============================================================================


void EnchantmentHelperBook::EnchantWithLevels::apply(uint64_t *rng, ItemStack *stack, int min, int max) {
    int rand = nextInt(rng, min, max);
    EnchantmentHelperBook::addRandomEnchantment(rng, stack, rand);
}


void EnchantmentHelperBook::EnchantWithLevels::apply(uint64_t *rng, ItemStack *stack, int level) {
    EnchantmentHelperBook::addRandomEnchantment(rng, stack, level);
}


//==============================================================================
//                              Enchantment Helpers
//==============================================================================


ELDataArray* EnchantmentHelperBook::buildEnchantmentList(uint64_t *rng, ItemStack *itemStackIn, int level) {

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

void EnchantmentHelperBook::addRandomEnchantment(uint64_t *const rng, ItemStack *const itemStackIn, int level) {
    ELDataArray* enchantmentVector = buildEnchantmentList(rng, itemStackIn, level);
    enchantmentVector->addEnchantments(itemStackIn);
}

