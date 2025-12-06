#include <gtest/gtest.h>

#include "rtss/task.h"

using rtss::Task;
using rtss::PeriodicTask;
using rtss::AperiodicTask;

namespace rtss {
    Task* Task::_idle = new Task(time::ZERO_DURATION, time::ZERO_DURATION);
}

TEST(TaskTest, DefaultConstructorInitializesIdAndIdleState) {
    Task t;

    // We assume default TimeDuration is zero.
    EXPECT_EQ(rtss::time::toInt(t.get_phase()), 0);
    EXPECT_EQ(rtss::time::toInt(t.get_wcet()), 0);
    EXPECT_EQ(t.get_id(), 0);
    EXPECT_TRUE(t.is_idle());
}

TEST(TaskTest, ValueConstructorSetsPhaseAndWcet) {
    auto phase = rtss::time::createTimeDurationMs(5);
    auto wcet = rtss::time::createTimeDurationMs(10);

    Task t(phase, wcet);

    EXPECT_EQ(rtss::time::toInt(t.get_phase()), 5);
    EXPECT_EQ(rtss::time::toInt(t.get_wcet()), 10);
    EXPECT_FALSE(t.is_idle());
}

TEST(TaskTest, SettersModifyPhaseAndWcet) {
    Task t(rtss::time::createTimeDurationMs(1),
           rtss::time::createTimeDurationMs(2));

    t.set_phase(rtss::time::createTimeDurationMs(100));
    t.set_wcet(rtss::time::createTimeDurationMs(200));

    EXPECT_EQ(rtss::time::toInt(t.get_phase()), 100);
    EXPECT_EQ(rtss::time::toInt(t.get_wcet()), 200);
}

TEST(TaskTest, IdCanBeSetOnlyOnce) {
    Task t(rtss::time::ZERO_DURATION,
           rtss::time::createTimeDurationMs(1));

    t.set_id(42);
    EXPECT_EQ(t.get_id(), 42);

    // Second attempt must throw
    EXPECT_THROW(t.set_id(7), std::runtime_error);
    // Value should remain unchanged
    EXPECT_EQ(t.get_id(), 42);
}

TEST(TaskTest, IdleReturnsSingletonIdleTask) {
    Task *idle1 = Task::Idle();
    Task *idle2 = Task::Idle();

    ASSERT_NE(idle1, nullptr);
    EXPECT_EQ(idle1, idle2); // singleton

    EXPECT_TRUE(idle1->is_idle());
    EXPECT_EQ(rtss::time::toInt(idle1->get_phase()), 0);
    EXPECT_EQ(rtss::time::toInt(idle1->get_wcet()), 0);
    EXPECT_EQ(idle1->get_id(), 0);
}

// ---- PeriodicTask tests ----------------------------------------------------
TEST(PeriodicTaskTest, DefaultConstructorCreatesZeroTask) {
    PeriodicTask pt;

    EXPECT_EQ(rtss::time::toInt(pt.get_phase()), 0);
    EXPECT_EQ(rtss::time::toInt(pt.get_wcet()), 0);
    EXPECT_EQ(rtss::time::toInt(pt.get_period()), 0);
    EXPECT_EQ(rtss::time::toInt(pt.get_rel_dl()), 0);
    EXPECT_TRUE(pt.is_idle());
}

TEST(PeriodicTaskTest, FullConstructorSetsAllFields) {
    auto phase = rtss::time::createTimeDurationMs(1);
    auto period = rtss::time::createTimeDurationMs(10);
    auto wcet = rtss::time::createTimeDurationMs(3);
    auto rel_dl = rtss::time::createTimeDurationMs(9);

    PeriodicTask pt(phase, period, wcet, rel_dl);

    EXPECT_EQ(rtss::time::toInt(pt.get_phase()), 1);
    EXPECT_EQ(rtss::time::toInt(pt.get_period()), 10);
    EXPECT_EQ(rtss::time::toInt(pt.get_wcet()), 3);
    EXPECT_EQ(rtss::time::toInt(pt.get_rel_dl()), 9);
    EXPECT_FALSE(pt.is_idle());
}

TEST(PeriodicTaskTest, PeriodWcetConstructorSetsDeadlineEqualToPeriod) {
    auto period = rtss::time::createTimeDurationMs(20);
    auto wcet = rtss::time::createTimeDurationMs(5);

    PeriodicTask pt(period, wcet);

    EXPECT_EQ(rtss::time::toInt(pt.get_phase()), 0);
    EXPECT_EQ(rtss::time::toInt(pt.get_period()), 20);
    EXPECT_EQ(rtss::time::toInt(pt.get_wcet()), 5);
    EXPECT_EQ(rtss::time::toInt(pt.get_rel_dl()), 20);
}

TEST(PeriodicTaskTest, SettersModifyPeriodAndDeadline) {
    PeriodicTask pt(rtss::time::createTimeDurationMs(10),
                    rtss::time::createTimeDurationMs(3));

    pt.set_period(rtss::time::createTimeDurationMs(50));
    pt.set_rel_dl(rtss::time::createTimeDurationMs(40));

    EXPECT_EQ(rtss::time::toInt(pt.get_period()), 50);
    EXPECT_EQ(rtss::time::toInt(pt.get_rel_dl()), 40);
}

TEST(PeriodicTaskTest, ToStringContainsKeyFields) {
    auto phase = rtss::time::createTimeDurationMs(1);
    auto period = rtss::time::createTimeDurationMs(10);
    auto wcet = rtss::time::createTimeDurationMs(3);
    auto rel_dl = rtss::time::createTimeDurationMs(9);

    PeriodicTask pt(phase, period, wcet, rel_dl);

    std::string s = pt.to_string();
    // Light-weight checks: just ensure numbers are present
    EXPECT_NE(s.find("phase"), std::string::npos);
    EXPECT_NE(s.find("period"), std::string::npos);
    EXPECT_NE(s.find("wcet"), std::string::npos);
    EXPECT_NE(s.find("rel_dl"), std::string::npos);
}

// ---- AperiodicTask tests ---------------------------------------------------

TEST(AperiodicTaskTest, ConstructorSetsArrivalAndWcet) {
    auto arrival = rtss::time::createTimeDurationMs(7);
    auto wcet = rtss::time::createTimeDurationMs(2);

    AperiodicTask at(arrival, wcet);

    EXPECT_EQ(rtss::time::toInt(at.get_arrival()), 7);
    EXPECT_EQ(rtss::time::toInt(at.get_wcet()), 2);
}

TEST(AperiodicTaskTest, ArrivalSetterAndGetterWork) {
    AperiodicTask at(rtss::time::createTimeDurationMs(1),
                     rtss::time::createTimeDurationMs(2));

    at.set_arrival(rtss::time::createTimeDurationMs(42));

    EXPECT_EQ(rtss::time::toInt(at.get_arrival()), 42);
}

TEST(AperiodicTaskTest, ToStringContainsArrivalAndWcet) {
    AperiodicTask at(rtss::time::createTimeDurationMs(12),
                     rtss::time::createTimeDurationMs(3));

    std::string s = at.to_string();
    EXPECT_NE(s.find("arrival"), std::string::npos);
    EXPECT_NE(s.find("wcet"), std::string::npos);
}
