#pragma once

#include <chrono>

/**
 * @file constants.hpp
 * @brief Defines mathematical and power-of-two constants used throughout the project.
 */

/**
 * @def PI
 * @brief The mathematical constant π (pi) with double precision.
 */
constexpr double PI = 3.141592653589793;

/**
 * @def PI_FLOAT
 * @brief The mathematical constant π (pi) with single precision.
 */
constexpr float PI_FLOAT = 3.1415927f;

/**
 * @def HALF_PI
 * @brief Half of the mathematical constant π (pi) with single precision.
 */
constexpr float HALF_PI_FLOAT = PI_FLOAT / 2.0F;

/**
 * @def TWO_PI
 * @brief Two times the mathematical constant π (pi) with double precision.
 */
constexpr double TWO_PI = 2.0 * PI;

/**
 * @def TWO_PI_FLOAT
 * @brief Two times the mathematical constant π (pi) with single precision.
 */
constexpr float TWO_PI_FLOAT = 2.0F * PI_FLOAT;

/**
 * @def TWO_THIRDS_PI_FLOAT
 * @brief Two-thirds of the mathematical constant π (pi) with single precision. (A third of a circle)
 */
constexpr float TWO_THIRDS_PI_FLOAT = 2.0F / 3.0F * PI_FLOAT;

/**
 * @def power2_31
 * @brief The value of 2^31, representing the maximum positive value for a signed 32-bit integer.
 */
#define power2_31 2147483648

/**
 * @def power2_32
 * @brief The value of 2^32, representing the range of an unsigned 32-bit integer.
 */
#define power2_32 4294967296

/**
 * @def power2_48
 * @brief The value of 2^48, often used in large integer calculations.
 */
#define power2_48 281474976710656LL

/**
 * @def power2_63
 * @brief The value of 2^63 - 1, representing the maximum positive value for a signed 64-bit integer.
 */
#define power2_63 9223090561878065151LL