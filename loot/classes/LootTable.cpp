#include "LootTable.hpp"


void LootTable::computeCumulativeWeights() {
    cumulativeWeights.resize(items.size());
    for (size_t i = 0; i < items.size(); ++i) {
        totalWeight += items[i].weight;
        cumulativeWeights[i] = totalWeight;
    }
}
