#ifndef RTSS_SCHEDULERS_RTSCHEDULER_H
#define RTSS_SCHEDULERS_RTSCHEDULER_H

#include <vector>

#include "rtss/task.h"

namespace rtss::schedulers {
    class RTScheduler {
    public:
        explicit RTScheduler(const std::vector<Task *> &tasks)
            : tasks(tasks) {
        }

        RTScheduler() = delete;
        virtual ~RTScheduler() = default;

        virtual void run_scheduler(size_t nperiods) = 0;

        RTScheduler(const RTScheduler &) = delete;

        RTScheduler &operator=(const RTScheduler &) = delete;

        RTScheduler(RTScheduler &&) = delete;

        RTScheduler &operator=(RTScheduler &&) = delete;

    protected:
        const std::vector<Task *> tasks;
    };
}

#endif