#pragma once

class UniformRoll {
public:
    int min;
    int max;
    UniformRoll() : min(0), max(0) {}
    UniformRoll(int min, int max) : min(min), max(max) {}
};
