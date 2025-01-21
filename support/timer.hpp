#pragma once

#include <chrono>


/// @brief A simple Timer class for measuring elapsed time using a steady clock.
/// @tparam Duration The duration type to represent elapsed time. Defaults to double seconds.
class Timer {
public:
    Timer() {
        start_time = std::chrono::steady_clock::now();
    }
    [[nodiscard]] double getSeconds() const {
        const auto end_time = std::chrono::steady_clock::now();
        const std::chrono::duration<double> elapsed = end_time - start_time;
        return elapsed.count();
    }
    [[maybe_unused]] void reset() {
        start_time = std::chrono::steady_clock::now();
    }

private:
    /// Type alias for the steady clock's time point
    using clock = std::chrono::steady_clock;
    using time_point = std::chrono::time_point<clock>;

    /// Starting time point
    time_point start_time;
};