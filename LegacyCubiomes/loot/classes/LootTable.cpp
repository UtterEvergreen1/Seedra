#include "LootTable.hpp"

void LootTable::computeCumulativeWeights() {
    cumulativeWeights.resize(items.size());
    int sum = 0;
    for (int i = 0; i < items.size(); ++i) {
        sum += items[i].weight;
        cumulativeWeights[i] = sum;
    }
    totalWeight = sum;
}
