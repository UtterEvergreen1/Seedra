#pragma once

#include "Pos2DTemplate.hpp"


class AreaRange {
public:
    struct iterator {
        Pos2D  p; // current point
        Pos2D  first, last, step;

        // output
        Pos2D  operator*()  const { return p; }

        // advance to next point
        iterator& operator++() {
            p.z += step.z;
            if (p.z == last.z + step.z) { // finished current row
                p.z  = first.z;
                p.x += step.x;
            }
            return *this;
        }

        bool operator!=(const iterator& other) const { return p.x != other.p.x; }
    };

    AreaRange(Pos2D centre, int radius,
              bool reverseX = true, bool reverseZ = true) {
        lower = centre - radius;
        upper = centre + radius - 1;

        step.x = reverseX ? -1 :  1;
        step.z = reverseZ ? -1 :  1;

        first.x = reverseX ? upper.x : lower.x;
        last .x = reverseX ? lower.x : upper.x;

        first.z = reverseZ ? upper.z : lower.z;
        last .z = reverseZ ? lower.z : upper.z;
    }

    MU ND Pos2D getLower() const { return lower; }
    MU ND Pos2D getUpper() const { return upper; }

    ND iterator begin() const { return {first, first, last, step}; }
    ND iterator end()   const { return {Pos2D{last.x+step.x, 0}, first, last, step}; }

private:
    Pos2D lower{}, upper{}, first{}, last{}, step{};
};