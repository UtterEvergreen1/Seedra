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
        lap_time = start_time;
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
     * @brief Gets the elapsed time in milliseconds since the Timer was created or last reset.
     * @return The elapsed time in milliseconds as a double.
     */
    [[nodiscard]] double getMilliseconds() const {
        return getSeconds() * 1000.0;
    }

    /**
     * @brief Gets the elapsed time since the last lap (or start if no lap has been called).
     *
     * This method returns the time elapsed since the last call to lap() (or since
     * construction if lap() hasn't been called yet), and then updates the lap time
     * to the current time for the next lap measurement.
     *
     * @return The elapsed time in seconds since the last lap as a double.
     */
    [[nodiscard]] double lap() {
        const auto current_time = std::chrono::steady_clock::now();
        const std::chrono::duration<double> elapsed = current_time - lap_time;
        lap_time = current_time;
        return elapsed.count();
    }

    /**
     * @brief Gets the elapsed time since the last lap without updating the lap time.
     *
     * Unlike lap(), this method does not update the lap time, allowing you to
     * peek at the current lap duration without affecting future lap() calls.
     *
     * @return The elapsed time in seconds since the last lap as a double.
     */
    [[nodiscard]] double peekLap() const {
        const auto current_time = std::chrono::steady_clock::now();
        const std::chrono::duration<double> elapsed = current_time - lap_time;
        return elapsed.count();
    }

    /**
     * @brief Resets the Timer to the current time.
     *
     * This method updates both the start time and lap time to the current time,
     * effectively restarting the measurement of elapsed time.
     */
    [[maybe_unused]] void reset() {
        start_time = std::chrono::steady_clock::now();
        lap_time = start_time;
    }

private:
    /// Type alias for the steady clock's time point
    using clock = std::chrono::steady_clock;
    using time_point = std::chrono::time_point<clock>;

    /// Starting time point
    time_point start_time; ///< The time point when the Timer was created or last reset.

    /// Lap time point for measuring intervals
    time_point lap_time; ///< The time point of the last lap measurement.
};
