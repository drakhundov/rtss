#ifndef RTSS_SCHEDULERS_CLOCK_BASED_H
#define RTSS_SCHEDULERS_CLOCK_BASED_H

#include <vector>

#include "rtss/tasktable.h"
#include "rtss/schedulers/RTScheduler.h"

namespace rtss::schedulers {
    class ClockBasedScheduler : public RTScheduler {
    public:
        ClockBasedScheduler(std::vector<Task *> &tasks,
                            TaskTable &task_tbl)
            : RTScheduler(tasks), task_tbl(task_tbl) {
        }

        void run_scheduler(size_t nperiods) override = 0;

    protected:
        TaskTable &task_tbl;
    };

    class TableDrivenScheduler : public ClockBasedScheduler {
    public:
        explicit TableDrivenScheduler(std::vector<Task *> &tasks,
                                      TaskTable &task_tbl)
            : ClockBasedScheduler(tasks, task_tbl) {
        }

        void run_scheduler(size_t nperiods) override;
    };
}

#endif
