#pragma once

#include <vector>
#include <string>

#include "rtss/task.h"


namespace rtss::io {
    struct Metadata {
        bool fully_periodic{false};
    };

    void read_task_list_from_csv(std::vector<Task *> &tasks, const std::string &file_path, Metadata &meta);
}
