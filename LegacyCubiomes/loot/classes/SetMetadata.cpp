#include "SetMetadata.hpp"

SetMetadata::SetMetadata(UniformRoll range, const std::vector<const Items::Item*>& dataItemsIn)
    : metaRange(range), dataItems(dataItemsIn) {}

void SetMetadata::apply(ItemStack& itemStack, RNG& random) {
    itemStack.item = dataItems[random.nextInt(this->metaRange.getMin(), this->metaRange.getMax())];
}