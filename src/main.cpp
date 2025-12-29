#include <iostream>
#include <vector>

#include "rtss/io/input.h"
#include "rtss/schedulers/dynamic.h"
#include "rtss/schedulers/static.h"

#include <fstream>

namespace rtss {
    std::unique_ptr<Task> Task::_idle = std::make_unique<Task>(time::TimeDuration::zero(), time::TimeDuration::zero());
}

using namespace rtss;

int main(int argc, char **argv) {
    std::vector<Task *> tasks;
    io::Metadata meta;
    std::filesystem::path tmp_dir = std::filesystem::temp_directory_path();
    std::filesystem::path csv_path = tmp_dir / "rtss_tasks.csv";
    std::cout << "Enter task set (one per line); End input with an empty line.\n\n" << std::flush;
    io::write_task_csv_from_stdin(csv_path);
    std::cout << "Tasks written to: " << csv_path << "\n";
    io::read_task_list_from_csv(tasks, csv_path.string(), meta);
    std::cout << "Loaded " << tasks.size() << "tasks.\n";
    std::cout << "Select scheduling algorithm:\n"
            "  1) Rate Monotonic (RM)\n"
            "  2) Deadline Monotonic (DM)\n"
            "  3) Earliest Deadline First (EDF)\n"
            "  4) Least Laxity First (LLF)\n"
            "  5) Table-Driven\n"
            "  6) Cyclic Executive\n"
            "Enter choice (1-6): ";
    int choice = 0;
    std::cin >> choice;
    // consume leftover newline so subsequent std::getline() in table input works
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    schedulers::RTScheduler *scheduler;
    switch (choice) {
        case 1: scheduler = new schedulers::RM(tasks);
            break;
        case 2: scheduler = new schedulers::DM(tasks);
            break;
        case 3: scheduler = new schedulers::EDF(tasks);
            break;
        case 4: scheduler = new schedulers::LLF(tasks);
            break;
        case 5: {
            std::filesystem::path tbl_path = tmp_dir / "rtss_task_table.csv";
            io::write_task_table_csv_from_stdin(tbl_path);
            TaskTableBuilder tbl_builder;
            io::read_task_table_from_csv(tbl_builder, tbl_path.string());
            TaskTable tbl = tbl_builder.build(StaticSchedulingMode::TASK_BASED);
            scheduler = new schedulers::TableDrivenScheduler(tasks, tbl);
            break;
        }
        case 6: {
            std::filesystem::path tbl_path = tmp_dir / "rtss_task_table.csv";
            io::write_task_table_csv_from_stdin(tbl_path);
            std::cout << "Table written to : " << tbl_path << "\n";
            TaskTableBuilder tbl_builder;
            io::read_task_table_from_csv(tbl_builder, tbl_path.string());
            std::cout << "Enter frame size (ms): ";
            int frame_ms;
            std::cin >> frame_ms;
            TaskTable tbl = tbl_builder.build(
                StaticSchedulingMode::FRAME_BASED,
                time::createTimeDurationMs(frame_ms),
                tasks
            );
            scheduler = new schedulers::CyclicExecutiveScheduler(tasks, tbl, frame_ms);
            break;
        }
        default:
            std::cerr << "Invalid choice.\n";
            return 1;
    }
    std::cout << "How many cycles? ";
    size_t ncycles;
    std::cin >> ncycles;
    scheduler->run_scheduler(ncycles);
    return 0;
}
