#include "Container.hpp"

#include <map>
#include <numeric>


void Container::printCombinedItems() const {
    std::map<const lce::items::Item*, int> itemCount;

    for (c_auto& itemStack: inventorySlots) {
        if (itemStack.stackSize > 0) {
            if (itemCount.contains(itemStack.item)) { itemCount[itemStack.item] += itemStack.stackSize; } else {
                itemCount[itemStack.item] = itemStack.stackSize;
            }
        }
    }

    for (c_auto& [fst, snd]: itemCount) {
        c_auto itemStack = ItemStack(fst, snd);
        std::cout << itemStack << std::endl;
    }
}


void Container::shuffleIntoContainer(std::vector<ItemStack>& items, MU const RNG& rng) {
    RNG rngState = rng;
    std::vector<int> slotOrder(numSlots);
    std::iota(slotOrder.begin(), slotOrder.end(), 0);

    std::vector<ItemStack> stackableItems;

    randomShuffle<int>(slotOrder, rngState);

    // Separate items with stackSize > 1 into stackableItems and keep others in items
    std::erase_if(items,
                  [&](const ItemStack& item) {
                      if (item.stackSize > 1) {
                          stackableItems.push_back(item);
                          return true;
                      }
                      return false;
                  });

    numSlots -= items.size();
    while (numSlots > 0 && !stackableItems.empty()) {
        c_int itemIndex = rngState.nextInt(0, static_cast<int>(stackableItems.size()) - 1);
        auto iter = std::next(stackableItems.begin(), itemIndex);
        ItemStack originalStack = std::move(*iter);

        stackableItems.erase(iter);

        c_int splitAmount = rngState.nextInt(1, originalStack.stackSize >> 1);
        ItemStack splittedStack = originalStack.splitStack(splitAmount);

        if (originalStack.stackSize == 0 || rngState.next(1) == 0) items.emplace_back(std::move(originalStack));
        else stackableItems.emplace_back(std::move(originalStack));

        if (splittedStack.stackSize == 0 || rngState.next(1) == 0) items.emplace_back(std::move(splittedStack));
        else stackableItems.emplace_back(std::move(splittedStack));
    }

    items.insert(items.end(), std::make_move_iterator(stackableItems.begin()),
                 std::make_move_iterator(stackableItems.end()));

    randomShuffle<ItemStack>(items, rngState);
    for (ItemStack& itemStack: items) {
        if (slotOrder.empty()) // Tried to over-fill a container
            return;

        if (itemStack.stackSize > 0) {
            c_int itemIndex = slotOrder.back();
            slotOrder.pop_back();
            setInventorySlotContents(itemIndex, std::move(itemStack));
        } else { slotOrder.pop_back(); }
    }
}
