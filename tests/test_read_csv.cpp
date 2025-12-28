#include <gtest/gtest.h>

#include <fstream>

#include "rtss/io/input.h"
#include "rtss/task.h"
#include "rtss/time.h"

namespace fs = std::filesystem;

class ReadTaskListFromCSVTest : public ::testing::Test {
protected:
    std::filesystem::path tmp;
    std::vector<rtss::Task*> parsed_task_lst;
    rtss::io::Metadata md;

    void SetUp() override {
        tmp = fs::temp_directory_path() / "rtss_test_tasks.csv";
        std::ofstream ofs(tmp);
        ofs << "type,phase,period,wcet,rel_dl\nP,0,5,1,5\nP,0,6,1,6\nP,0,10,2,10\nP,0,15,3,15\n";
        ofs.close();
        ASSERT_NO_THROW(rtss::io::read_task_list_from_csv(parsed_task_lst, tmp.string(), md));
    }

    void TearDown() override {
        for (auto* t : parsed_task_lst) {
            delete t;
        }
        std::error_code ec;
        std::filesystem::remove(tmp, ec);
    }
};

TEST_F(ReadTaskListFromCSVTest, TaskListSizeIsCorrect) {
    EXPECT_EQ(parsed_task_lst.size(), 4u);
}

TEST_F(ReadTaskListFromCSVTest, MetadataIsCorrect) {
    EXPECT_TRUE(md.fully_periodic);
}

TEST_F(ReadTaskListFromCSVTest, ParsedContentIsCorrect) {
    std::vector<rtss::Task *> raw_tasks = {
        new rtss::PeriodicTask(rtss::time::createTimeDurationMs(0), rtss::time::createTimeDurationMs(5),
                               rtss::time::createTimeDurationMs(1), rtss::time::createTimeDurationMs(5)),
        new rtss::PeriodicTask(rtss::time::createTimeDurationMs(0), rtss::time::createTimeDurationMs(6),
                               rtss::time::createTimeDurationMs(1), rtss::time::createTimeDurationMs(6)),
        new rtss::PeriodicTask(rtss::time::createTimeDurationMs(0), rtss::time::createTimeDurationMs(10),
                               rtss::time::createTimeDurationMs(2), rtss::time::createTimeDurationMs(10)),
        new rtss::PeriodicTask(rtss::time::createTimeDurationMs(0), rtss::time::createTimeDurationMs(15),
                               rtss::time::createTimeDurationMs(3), rtss::time::createTimeDurationMs(15))
    };
    raw_tasks[0]->set_id(1);
    raw_tasks[1]->set_id(2);
    raw_tasks[2]->set_id(3);
    raw_tasks[3]->set_id(4);
    for (size_t i = 0; i < parsed_task_lst.size(); i++) {
        EXPECT_EQ(parsed_task_lst[i]->to_string(), raw_tasks[i]->to_string());
    }
}

class ReadTaskTableFromCSVTest : public ::testing::Test {
protected:
    std::filesystem::path tmp;
    rtss::TaskTableBuilder tbl_builder;
    std::unique_ptr<rtss::TaskTable> tbl;
    rtss::io::Metadata md;

    void SetUp() override {
        tmp = fs::temp_directory_path() / "rtss_test_task_table.csv";
        std::ofstream ofs(tmp);
        ofs << "time,task_id\n0,1\n1,2\n2,3\n3,0\n4,4\n7,1\n8,2\n9,1\n10,3\n12,2\n13,1\n14,4\n17,0\n20,3\n22,1\n23,4\n26,0\n26,1\n27,2\n28,1\n30,-1\n";
        ofs.close();
        ASSERT_NO_THROW(rtss::io::read_task_table_from_csv(tbl_builder, tmp.string()));
        tbl = std::make_unique<rtss::TaskTable>(tbl_builder.build(rtss::SchedulingMode::TASK_BASED));
    }

    void TearDown() override {
        std::error_code ec;
        std::filesystem::remove(tmp, ec);
    }
};

TEST_F(ReadTaskTableFromCSVTest, ParsedContentIsCorrect) {
    std::vector raw_schedule = {
        rtss::TaskScheduleEntry(rtss::time::createTimeDurationMs(0), 1),
        rtss::TaskScheduleEntry(rtss::time::createTimeDurationMs(1), 2),
        rtss::TaskScheduleEntry(rtss::time::createTimeDurationMs(2), 3),
        rtss::TaskScheduleEntry(rtss::time::createTimeDurationMs(3), 0),
        rtss::TaskScheduleEntry(rtss::time::createTimeDurationMs(4), 4),
        rtss::TaskScheduleEntry(rtss::time::createTimeDurationMs(7), 1),
        rtss::TaskScheduleEntry(rtss::time::createTimeDurationMs(8), 2),
        rtss::TaskScheduleEntry(rtss::time::createTimeDurationMs(9), 1),
        rtss::TaskScheduleEntry(rtss::time::createTimeDurationMs(10), 3),
        rtss::TaskScheduleEntry(rtss::time::createTimeDurationMs(12), 2),
        rtss::TaskScheduleEntry(rtss::time::createTimeDurationMs(13), 1),
        rtss::TaskScheduleEntry(rtss::time::createTimeDurationMs(14), 4),
        rtss::TaskScheduleEntry(rtss::time::createTimeDurationMs(17), 0),
        rtss::TaskScheduleEntry(rtss::time::createTimeDurationMs(20), 3),
        rtss::TaskScheduleEntry(rtss::time::createTimeDurationMs(22), 1),
        rtss::TaskScheduleEntry(rtss::time::createTimeDurationMs(23), 4),
        rtss::TaskScheduleEntry(rtss::time::createTimeDurationMs(26), 0),
        rtss::TaskScheduleEntry(rtss::time::createTimeDurationMs(26), 1),
        rtss::TaskScheduleEntry(rtss::time::createTimeDurationMs(27), 2),
        rtss::TaskScheduleEntry(rtss::time::createTimeDurationMs(28), 1),
        rtss::TaskScheduleEntry(rtss::time::createTimeDurationMs(30), -1)
    };
    EXPECT_EQ(tbl->size(), raw_schedule.size());
    for (size_t i = 0; i < tbl->size(); i++) {
        const rtss::TaskScheduleEntry &parsed_entry = tbl->get_current_entry();
        EXPECT_EQ(parsed_entry.start_time, raw_schedule[i].start_time);
        EXPECT_EQ(parsed_entry.task_id, raw_schedule[i].task_id);
        tbl->increment_k();
    }
}
