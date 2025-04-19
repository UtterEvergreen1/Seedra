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
#define PI 3.141592653589793

/**
 * @def PI_FLOAT
 * @brief The mathematical constant π (pi) with single precision.
 */
#define PI_FLOAT 3.1415927F

/**
 * @def HALF_PI
 * @brief Half of the mathematical constant π (pi) with single precision.
 */
#define HALF_PI 1.5707964F

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