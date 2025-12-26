#include <gtest/gtest.h>

#include <vector>

#include "rtss/frame.h"
#include "rtss/task.h"

namespace {
    using namespace rtss;

    // A simple test double for Task.
    // You MUST adjust the base-class constructor call to match your Task's actual ctor.
    class TestTask : public Task {
    public:
        explicit TestTask(int16_t id)
            : Task(time::createTimeDurationMs(5), time::createTimeDurationMs(3)) {
            this->set_id(id);
        }

        void run_task(time::TimeDuration exec_tm) override {
            run_calls.push_back(exec_tm);
            total_run_time += exec_tm;
        }

    private:
        std::vector<time::TimeDuration> run_calls;
        time::TimeDuration total_run_time{time::ZERO_DURATION};
    };
}

// ------------------ FrameJob tests ------------------

TEST(FrameJobTest, ToStringFormatsCorrectly) {
    FrameJob job(3, time::createTimeDurationMs(5));
    std::string s = job.to_string();
    EXPECT_EQ(s, "{T3, 5ms}");
}

// ------------------ Frame tests ------------------

TEST(FrameTest, ToStringAggregatesAllJobsWithNewlines) {
    std::vector<FrameJob> jobs;
    jobs.emplace_back(1, time::createTimeDurationMs(2));
    jobs.emplace_back(2, time::createTimeDurationMs(3));

    Frame f(std::move(jobs));
    std::string s = f.to_string();

    EXPECT_NE(s.find("{T1, 2ms}\n"), std::string::npos);
    EXPECT_NE(s.find("{T2, 3ms}\n"), std::string::npos);
}

TEST(FrameTest, RunFrameThrowsIfNoJobs) {
    Frame f(std::vector<FrameJob>{});

    std::vector<Task *> tasks; // empty, not used
    EXPECT_THROW(f.run_frame(tasks), std::runtime_error);
}

TEST(FrameTest, RunFrameRunsCorrectTasksWithCorrectDurations) {
    // Two test tasks
    TestTask t1(1);
    TestTask t2(2);

    std::vector<Task *> tasks_ref = {&t1, &t2};

    // Frame has: T1 for 3 time::createTimeDurationMs, then T2 for 5 time::createTimeDurationMs, then IDLE for 2 time::createTimeDurationMs
    std::vector<FrameJob> jobs;
    jobs.emplace_back(1, time::createTimeDurationMs(3));
    jobs.emplace_back(2, time::createTimeDurationMs(5));
    jobs.emplace_back(static_cast<int16_t>(TaskID::IDLE), time::createTimeDurationMs(2));

    Frame f(std::move(jobs));

    // Note: Idle tasks go through Task::Idle(), so they don't affect t1/t2.
    f.run_frame(tasks_ref);

    // T1 should have one call with 3 time::createTimeDurationMs
    ASSERT_EQ(t1.run_calls.size(), 1u);
    EXPECT_EQ(time::toInt(t1.run_calls[0]), 3);
    EXPECT_EQ(time::toInt(t1.total_run_time), 3);

    // T2 should have one call with 5 time::createTimeDurationMs
    ASSERT_EQ(t2.run_calls.size(), 1u);
    EXPECT_EQ(time::toInt(t2.run_calls[0]), 5);
    EXPECT_EQ(time::toInt(t2.total_run_time), 5);
}

// ------------------ FrameContainer tests ------------------

TEST(FrameContainerTest, SizeAndEmptyReflectNumberOfFrames) {
    TestTask t1(1);
    std::vector<Task *> tasks = {&t1};

    std::vector<Frame> frames;
    frames.emplace_back(std::vector<FrameJob>{FrameJob(1, time::createTimeDurationMs(5))});
    frames.emplace_back(std::vector<FrameJob>{FrameJob(1, time::createTimeDurationMs(2))});

    FrameContainer fc(std::move(tasks), std::move(frames), time::createTimeDurationMs(5));

    EXPECT_EQ(fc.size(), 2u);
    EXPECT_FALSE(fc.empty());
}

TEST(FrameContainerTest, GetKthFrameThrowsOnInvalidIndex) {
    TestTask t1(1);
    std::vector<Task *> tasks = {&t1};

    std::vector<Frame> frames;
    frames.emplace_back(std::vector<FrameJob>{FrameJob(1, time::createTimeDurationMs(5))});

    FrameContainer fc(std::move(tasks), std::move(frames), time::createTimeDurationMs(5));

    EXPECT_THROW(fc.get_kth_frame(1), std::out_of_range);
}

TEST(FrameContainerTest, RunFrameThrowsOnInvalidIndex) {
    TestTask t1(1);
    std::vector<Task *> tasks = {&t1};

    std::vector<Frame> frames;
    frames.emplace_back(std::vector<FrameJob>{FrameJob(1, time::createTimeDurationMs(5))});

    FrameContainer fc(std::move(tasks), std::move(frames), time::createTimeDurationMs(5));

    EXPECT_THROW(fc.run_frame(1), std::out_of_range);
}

TEST(FrameContainerTest, RunFrameDelegatesToCorrectFrameAndTasks) {
    TestTask t1(1);
    TestTask t2(2);

    std::vector<Task *> tasks = {&t1, &t2};

    // Frame 0: T1 for 3 time::createTimeDurationMs
    std::vector<FrameJob> jobs0;
    jobs0.emplace_back(1, time::createTimeDurationMs(3));

    // Frame 1: T2 for 4 time::createTimeDurationMs, then T1 for 1 time::createTimeDurationMs
    std::vector<FrameJob> jobs1;
    jobs1.emplace_back(2, time::createTimeDurationMs(4));
    jobs1.emplace_back(1, time::createTimeDurationMs(1));

    std::vector<Frame> frames;
    frames.emplace_back(std::move(jobs0));
    frames.emplace_back(std::move(jobs1));

    FrameContainer fc(std::move(tasks), std::move(frames), time::createTimeDurationMs(5));

    // Run frame 0
    fc.run_frame(0);
    EXPECT_EQ(time::toInt(t1.total_run_time), 3);
    EXPECT_EQ(time::toInt(t2.total_run_time), 0);

    // Run frame 1
    fc.run_frame(1);
    EXPECT_EQ(time::toInt(t1.total_run_time), 3 + 1);
    EXPECT_EQ(time::toInt(t2.total_run_time), 4);

    ASSERT_EQ(t1.run_calls.size(), 2u);
    EXPECT_EQ(time::toInt(t1.run_calls[0]), 3);
    EXPECT_EQ(time::toInt(t1.run_calls[1]), 1);

    ASSERT_EQ(t2.run_calls.size(), 1u);
    EXPECT_EQ(time::toInt(t2.run_calls[0]), 4);
}

TEST(FrameContainerTest, ToStringListsAllFramesAndJobs) {
    TestTask t1(1);
    std::vector<Task *> tasks = {&t1};

    std::vector<Frame> frames;
    frames.emplace_back(std::vector<FrameJob>{FrameJob(1, time::createTimeDurationMs(3))});
    frames.emplace_back(std::vector<FrameJob>{FrameJob(1, time::createTimeDurationMs(2))});

    FrameContainer fc(std::move(tasks), std::move(frames), time::createTimeDurationMs(5));

    std::string s = fc.to_string();
    EXPECT_NE(s.find("Frame 0:\n"), std::string::npos);
    EXPECT_NE(s.find("Frame 1:\n"), std::string::npos);
    EXPECT_NE(s.find("{T1, 3ms}"), std::string::npos);
    EXPECT_NE(s.find("{T1, 2ms}"), std::string::npos);
}
