#pragma once

#include <chrono>

/**
 * @class Timer
 * @brief A simple Timer class for measuring elapsed time using a steady clock.
 *
 * This class provides functionality to measure the time elapsed since its creation
 * or since the last reset. It uses `std::chrono::steady_clock` to ensure a monotonic
 * clock, making it suitable for measuring intervals.
 */
class Timer {
public:
    /**
     * @brief Constructs a Timer object and initializes the start time.
     */
    Timer() {
        start_time = std::chrono::steady_clock::now();
    }

    /**
     * @brief Gets the elapsed time in seconds since the Timer was created or last reset.
     * @return The elapsed time in seconds as a double.
     */
    [[nodiscard]] double getSeconds() const {
        const auto end_time = std::chrono::steady_clock::now();
        const std::chrono::duration<double> elapsed = end_time - start_time;
        return elapsed.count();
    }

    /**
     * @brief Resets the Timer to the current time.
     *
     * This method updates the start time to the current time, effectively restarting
     * the measurement of elapsed time.
     */
    [[maybe_unused]] void reset() {
        start_time = std::chrono::steady_clock::now();
    }

private:
    /// Type alias for the steady clock's time point
    using clock = std::chrono::steady_clock;
    using time_point = std::chrono::time_point<clock>;

    /// Starting time point
    time_point start_time; ///< The time point when the Timer was created or last reset.
};
