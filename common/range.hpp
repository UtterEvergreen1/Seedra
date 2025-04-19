#pragma once

#include <ostream>


/**
 * @struct Range
 * @brief Represents a cuboidal range used to define a generation area in 2D or 3D space.
 *
 * Cuboidal range, given by a position, size, and scaling in the horizontal
 * axes, used to define a generation range. The parameters for the vertical
 * control can be left at zero when dealing with versions without 3D volume
 * support. The vertical scaling is equal to 1:1 iff scale == 1, and 1:4
 * (default biome scale) in all other cases!
 *
 * @param scale Horizontal scale factor, should be one of 1, 4, 16, 64, or 256.
 *              Additionally, a value of zero bypasses scaling and expects a
 *              manual generation entry layer.
 * @param x Horizontal position, i.e. coordinates of the northwest corner.
 * @param z Horizontal position, i.e. coordinates of the northwest corner.
 * @param sx Horizontal size (width and height for 2D), should be positive.
 * @param sz Horizontal size (width and height for 2D), should be positive.
 * @param y Vertical position (for 3D volume).
 * @param sy Vertical size (for 3D volume).
 *
 * Volumes generated with a range are generally indexed as:
 * out[i_z * sx + i_x] where i_x, i_z are indices in their respective directions.
 *
 * EXAMPLES
 * Area at normal biome scale (1:4):
 * Range r_2d = {4, x, z, sx, sz};
 * (C99 syntax allows commission of the trailing zero-initialization.)
 *
 * Area at block scale (1:1) at sea level:
 * Range r_surf = {1, x, z, sx, sz, 63};
 * (Block level scale uses Voronoi sampling with 1:1 vertical scaling.)
 *
 * Area at chunk scale (1:16) near sea level:
 * Range r_surf16 = {16, x, z, sx, sz, 15};
 * (Note that the vertical scaling is always 1:4 for non-Voronoi scales.)
 *
 * Volume at scale (1:4):
 * Range r_vol = {4, x, z, sx, sz, y, sy};
 */
struct Range {
    int scale; ///< Horizontal scale factor (e.g., 1, 4, 16, 64, or 256). A value of 0 bypasses scaling.
    int x;     ///< Horizontal x-coordinate of the northwest corner.
    int z;     ///< Horizontal z-coordinate of the northwest corner.
    int sx;    ///< Horizontal size (width) of the range. Must be positive.
    int sz;    ///< Horizontal size (height) of the range. Must be positive.

    /**
     * @brief Outputs the Range object to an output stream in a readable format.
     * @param out The output stream.
     * @param range The Range object to output.
     * @return The modified output stream.
     */
    friend std::ostream& operator<<(std::ostream& out, const Range& range) {
        out << "Range{" << range.scale << ", " << range.x << ", " << range.z << ", " << range.sx << ", " << range.sz
            << "}";
        return out;
    }
};