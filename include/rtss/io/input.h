#pragma once

#include <vector>
#include <filesystem>
#include <string>

#include "rtss/task.h"
#include "rtss/tasktable.h"

namespace rtss::io {
    struct Metadata {
        bool fully_periodic{false};
    };

    void read_task_list_from_csv(std::vector<Task *> &tasks, const std::string &file_path, Metadata &meta);

    void read_task_list_from_csv(std::vector<PeriodicTask *> &periodic, std::vector<AperiodicTask *> &aperiodic,
                                 const std::string &file_path, Metadata &meta);

    //* In the CSV file
    //* '0' stands for Idle
    //* '-1' stands for "reset timer"
    void read_task_table_from_csv(TaskTableBuilder &tbl_builder, const std::string &file_path);

    void write_task_csv_from_stdin(const std::filesystem::path &csv_path);

    void write_task_table_csv_from_stdin(const std::filesystem::path &csv_path);
}
