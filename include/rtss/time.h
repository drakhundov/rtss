#ifndef RTSS__TIME_H
#define RTSS__TIME_H

#include <chrono>

// Use milliseconds for now.


namespace rtss::time {
    using Clock = std::chrono::steady_clock;
    using TimeDuration = Clock::duration;
    using TimePoint = Clock::time_point;

    inline TimeDuration createTimeDurationMs(int ms) {
        return std::chrono::milliseconds(ms);
    }

    inline int64_t toInt(const TimeDuration &td) {
        return std::chrono::duration_cast<std::chrono::milliseconds>(td).count();
    }

    inline int64_t toInt(std::chrono::time_point<std::chrono::steady_clock> td) {
        return std::chrono::duration_cast<std::chrono::milliseconds>(td.time_since_epoch()).count();
    }

    const TimeDuration ZERO_DURATION = TimeDuration::zero();

}


#endif
