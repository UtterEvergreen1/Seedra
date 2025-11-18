#include "MathHelper.hpp"

const std::array<float, MathHelper::TABLE_SIZE> MathHelper::SIN_TABLE = [] {
    std::array<float, MathHelper::TABLE_SIZE> table{};
    for (size_t i = 0; i < MathHelper::TABLE_SIZE; ++i) {
        table[i] = sinf(static_cast<float>(i) / MathHelper::CONVERSION);
    }
    return table;
}();
