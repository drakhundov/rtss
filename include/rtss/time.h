#ifndef RTSS__TIME_H
#define RTSS__TIME_H

#include <chrono>

// Use milliseconds for now.

namespace rtss {
    namespace time {
        using Clock = std::chrono::steady_clock;
        using TimeDuration = Clock::duration;
        using TimePoint = Clock::time_point;

        inline TimeDuration createTimeDurationMs(int ms) {
            return std::chrono::milliseconds(ms);
        }
    }
}

#endif
