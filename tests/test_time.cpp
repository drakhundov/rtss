#include <gtest/gtest.h>

#include "rtss/time.h"

TEST(Time, CreateTimeDurationMs_ExactValue) {
    int ms = 1500;
    rtss::time::TimeDuration d = rtss::time::createTimeDurationMs(ms);
    auto counted = std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
    EXPECT_EQ(counted, ms);
}
