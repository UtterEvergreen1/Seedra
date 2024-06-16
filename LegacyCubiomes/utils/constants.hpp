#pragma once

#include <chrono>

#define PI 3.141592653589793
#define PI_FLOAT 3.1415927F
#define HALF_PI 1.5707964F
#define power2_31 2147483648
#define power2_32 4294967296
#define power2_48 281474976710656LL
#define power2_63 9223090561878065151LL

static u64 getNanoSeconds() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
}
static u64 getMilliseconds() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
}
static u64 getSeconds() {
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
}
