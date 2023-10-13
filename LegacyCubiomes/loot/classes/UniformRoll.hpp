#pragma once

class UniformRoll {
private:
    /// holds both the min and max.
    uint8_t val;
public:
    UniformRoll() : val(0) {}
    UniformRoll(uint8_t min, uint8_t max) : val((min << 4) | (max & 0x0F)) {}
    ND inline uint8_t getMin() const {return val >> 4;}
    ND inline uint8_t getMax() const {return val & 15;}
};

