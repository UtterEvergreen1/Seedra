#pragma once

#include "terrain/World.hpp"
#include "lce/enums.hpp"
#include "common/Pos3DTemplate.hpp"
#include "common/rng.hpp"
#include "terrain/generator.hpp"

/**
 * @class AbstractWorldGenerator
 * @brief An abstract base class for world generation.
 * Provides an interface for generating features in a world and utility methods for position generation.
 */
class AbstractWorldGenerator {
public:
    /**
     * @brief Virtual destructor for AbstractWorldGenerator.
     */
    virtual ~AbstractWorldGenerator();

    /**
     * @brief Pure virtual method to generate features in the world.
     * @param worldIn Pointer to the World object where features will be generated.
     * @param rng Reference to the random number generator.
     * @param pos The position where the generation starts.
     * @return True if the generation was successful, false otherwise.
     */
    virtual bool generate(World *worldIn, RNG &rng, const Pos3D &pos) const = 0;

    /**
     * @brief Generates a random 3D position within specified ranges.
     * If the console is not Xbox, the X and Z coordinates are generated first; otherwise, Z and X are swapped.
     * @tparam xRange The range for the X-coordinate.
     * @tparam yRange The range for the Y-coordinate. If 0, Y-coordinate is not generated.
     * @tparam zRange The range for the Z-coordinate.
     * @param worldIn Pointer to the World object.
     * @param rng Reference to the random number generator.
     * @return A randomly generated 3D position.
     */
    template<int xRange, int yRange, int zRange>
    MU static Pos3D getWorldGenPos3D(World *worldIn, RNG &rng) {
        Pos3D pos;
        if (worldIn->getGenerator()->getConsole() != lce::CONSOLE::XBOX360 &&
            worldIn->getGenerator()->getConsole() != lce::CONSOLE::XBOX1) {
            pos.x = rng.nextInt(xRange) - rng.nextInt(xRange);
            if constexpr (yRange != 0) { pos.y = rng.nextInt(yRange) - rng.nextInt(yRange); }
            pos.z = rng.nextInt(zRange) - rng.nextInt(zRange);
        } else {
            pos.z = rng.nextInt(zRange) - rng.nextInt(zRange);
            if constexpr (yRange != 0) { pos.y = rng.nextInt(yRange) - rng.nextInt(yRange); }
            pos.x = rng.nextInt(xRange) - rng.nextInt(xRange);
        }
        return pos;
    }

    /**
     * @brief Generates a random 3D position within specified ranges.
     * If the console is not Xbox, the X and Z coordinates are generated first; otherwise, Z and X are swapped.
     * @param worldIn Pointer to the World object.
     * @param rng Reference to the random number generator.
     * @param xRange The range for the X-coordinate.
     * @param yRange The range for the Y-coordinate. If 0, Y-coordinate is not generated.
     * @param zRange The range for the Z-coordinate.
     * @return A randomly generated 3D position.
     */
    MU static Pos3D getWorldGenPos3D(World *worldIn, RNG &rng, c_int xRange, c_int yRange, c_int zRange) {
        Pos3D pos;
        if (worldIn->getGenerator()->getConsole() != lce::CONSOLE::XBOX360 &&
            worldIn->getGenerator()->getConsole() != lce::CONSOLE::XBOX1) {
            pos.x = rng.nextInt(xRange) - rng.nextInt(xRange);
            if (yRange != 0) { pos.y = rng.nextInt(yRange) - rng.nextInt(yRange); }
            pos.z = rng.nextInt(zRange) - rng.nextInt(zRange);
        } else {
            pos.z = rng.nextInt(zRange) - rng.nextInt(zRange);
            if (yRange != 0) { pos.y = rng.nextInt(yRange) - rng.nextInt(yRange); }
            pos.x = rng.nextInt(xRange) - rng.nextInt(xRange);
        }
        return pos;
    }
};
