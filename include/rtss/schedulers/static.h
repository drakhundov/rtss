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
            if (task_tbl.scheduling_mode() != StaticSchedulingMode::TASK_BASED) {
                throw std::runtime_error(
                    "[CyclicExecutiveScheduler::CyclicExecutiveScheduler] TaskTable must be in TASK_BASED mode");
            }
        }

        void run_scheduler(size_t nperiods) override;
    };

    class CyclicExecutiveScheduler : public ClockBasedScheduler {
    public:
        explicit CyclicExecutiveScheduler(std::vector<Task *> tasks,
                                          TaskTable &task_tbl,
                                          int frame_sz)
            : ClockBasedScheduler(tasks, task_tbl), frame_sz(frame_sz) {
            if (task_tbl.scheduling_mode() != StaticSchedulingMode::FRAME_BASED) {
                throw std::runtime_error(
                    "[CyclicExecutiveScheduler::CyclicExecutiveScheduler] TaskTable must be in FRAME_BASED mode");
            }
        }

        // TODO: constructor that determines frame size automatically.

        void run_scheduler(size_t nperiods) override;

    private:
        int frame_sz{0};
    };
}

#endif
