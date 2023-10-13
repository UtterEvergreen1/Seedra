#include "Container.hpp"

void Container::printCombinedItems() {
    std::map<const Items::Item *, int> itemCount;

    for (const auto &itemStack: inventorySlots) {
        if (itemStack.stackSize > 0) {
            if (itemCount.find(itemStack.item) != itemCount.end()) {
                itemCount[itemStack.item] += itemStack.stackSize;
            } else {
                itemCount[itemStack.item] = itemStack.stackSize;
            }
        }
    }

    for (const auto& pair : itemCount) {
        ItemStack itemStack = ItemStack(pair.first, pair.second);
        std::cout << itemStack << std::endl;
    }
}





void Container::shuffleIntoContainer(std::vector<ItemStack>& items, RNG& rng) {
    RNG rngState = rng;
    std::vector<int> slotOrder(numSlots);
    std::iota(slotOrder.begin(), slotOrder.end(), 0);

    std::vector<ItemStack> stackableItems;

    randomShuffle<int>(slotOrder, rngState);

    // Separate items with stackSize > 1 into stackableItems and keep others in items
    items.erase(std::remove_if(items.begin(), items.end(),
                               [&](const ItemStack& item) {
                                   if (item.stackSize > 1) {
                                       stackableItems.push_back(item);
                                       return true;
                                   }
                                   return false;
                               }), items.end());

    numSlots -= items.size();
    while(numSlots > 0 && !stackableItems.empty()) {
        int itemIndex = rngState.nextInt(0, (int)stackableItems.size() - 1);
        auto iter = std::next(stackableItems.begin(), itemIndex);
        ItemStack originalStack = std::move(*iter);

        stackableItems.erase(iter);

        int splitAmount = rngState.nextInt(1, originalStack.stackSize >> 1);
        ItemStack splittedStack = originalStack.splitStack(splitAmount);

        if (originalStack.stackSize == 0 || rngState.next(1) == 0)
            items.emplace_back(std::move(originalStack));
        else
            stackableItems.emplace_back(std::move(originalStack));

        if (splittedStack.stackSize == 0 || rngState.next(1) == 0)
            items.emplace_back(std::move(splittedStack));
        else
            stackableItems.emplace_back(std::move(splittedStack));
    }

    items.insert(items.end(), std::make_move_iterator(stackableItems.begin()), std::make_move_iterator(stackableItems.end()));

    randomShuffle<ItemStack>(items, rngState);
    for (ItemStack& itemStack : items) {
        if (slotOrder.empty()) // Tried to over-fill a container
            return;

        if (itemStack.stackSize > 0) {
            int itemIndex = slotOrder.back();
            slotOrder.pop_back();
            setInventorySlotContents(itemIndex, std::move(itemStack));
        }
        else {
            slotOrder.pop_back();
        }
    }
}

