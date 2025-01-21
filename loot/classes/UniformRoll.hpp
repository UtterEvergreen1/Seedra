#pragma once

class UniformRoll {
    /// holds both the min and max.
    u16 val = 0;

public:
    UniformRoll() = default;
    UniformRoll(c_u8 min, u8 const max) : val(min << 8 | (max & 0xff)) {}
    [[nodiscard]] u8 getMin() const { return val >> 8; }
    [[nodiscard]] u8 getMax() const { return val & 0xff; }
};
