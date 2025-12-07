#include <gtest/gtest.h>

#include <fstream>

#include "rtss/io/input.h"
#include "rtss/task.h"

namespace fs = std::filesystem;

class ReadTaskListFromCSVTest : public ::testing::Test {
protected:
    std::filesystem::path tmp;
    std::vector<rtss::Task*> parsed_task_lst;
    rtss::io::Metadata md;

    void SetUp() override {
        tmp = fs::temp_directory_path() / "rtss_test_tasks.csv";
        std::ofstream ofs(tmp);
        ofs << "type,phase,period,wcet,rel_dl\n"
            << "P,0,5,1,5\n"
            << "P,0,6,1,6\n"
            << "P,0,10,2,10\n"
            << "P,0,15,3,15\n";
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
        std::cout << "ID: " << parsed_task_lst[i]->get_id() << std::endl;
        EXPECT_EQ(parsed_task_lst[i]->to_string(), raw_tasks[i]->to_string());
    }
}
