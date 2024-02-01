#pragma once

class UniformRoll {
    /// holds both the min and max.
    uint8_t val = 0;

public:
    UniformRoll() = default;
    UniformRoll(const uint8_t min, uint8_t const max) : val(min << 4 | max & 0x0F) {}
    [[nodiscard]] uint8_t getMin() const { return val >> 4; }
    [[nodiscard]] uint8_t getMax() const { return val & 15; }
};
