#pragma once

class UniformRoll {
    /// holds both the min and max.
    uint16_t val = 0;

public:
    UniformRoll() = default;
    UniformRoll(const uint8_t min, uint8_t const max) : val(min << 8 | max & 0xff) {}
    [[nodiscard]] uint8_t getMin() const { return val >> 8; }
    [[nodiscard]] uint8_t getMax() const { return val & 0xff; }
};
