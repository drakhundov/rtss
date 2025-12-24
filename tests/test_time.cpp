#include <gtest/gtest.h>

#include "rtss/time.h"

TEST(Time, CreateTimeDurationMs_ReturnsCorrectDuration) {
    int time_ms = 1500;
    rtss::time::TimeDuration d = rtss::time::createTimeDurationMs(time_ms);
    auto counted = std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
    EXPECT_EQ(counted, time_ms);
}

TEST(Time, ToInt_ReturnsCorrectMilliseconds) {
    rtss::time::TimeDuration d = std::chrono::milliseconds(2500);
    int64_t time_ms = rtss::time::toInt(d);
    EXPECT_EQ(time_ms, 2500);
}
