#ifndef RTSS_SCHEDULERS_PRIORITY_BASED_H
#define RTSS_SCHEDULERS_PRIORITY_BASED_H

#include <vector>

#include "rtss/schedulers/RTScheduler.h"

namespace rtss::schedulers {
    class PriorityBasedScheduler : public RTScheduler {
    public:
        explicit PriorityBasedScheduler(std::vector<Task *> &tasks)
            : RTScheduler(tasks) {
        }

        int get_task_priority(size_t task_id) { return priorities[task_id]; }

        void set_task_priority(size_t task_id, int priority) { priorities[task_id] = priority; }

        void run_scheduler(size_t nperiods) override;

    private:
        std::vector<int> priorities{0};

    protected:
        void assign_priorities();

        virtual bool compare(PeriodicTask *P1, PeriodicTask *P2) = 0;
    };

    class RateMonotonicScheduler : public PriorityBasedScheduler {
    public:
        explicit RateMonotonicScheduler(std::vector<Task *> &tasks)
            : PriorityBasedScheduler(tasks) {
            PriorityBasedScheduler::assign_priorities();
        }

    private:
        bool compare(PeriodicTask *P1, PeriodicTask *P2) override;
    };

    class DeadlineMonotonicScheduler : public PriorityBasedScheduler {
    public:
        explicit DeadlineMonotonicScheduler(std::vector<Task *> &tasks)
            : PriorityBasedScheduler(tasks) {
            PriorityBasedScheduler::assign_priorities();
        }

    private:
        bool compare(PeriodicTask *P1, PeriodicTask *P2) override;
    };

    class EarliestDeadlineFirstScheduler : public PriorityBasedScheduler {
    public:
        explicit EarliestDeadlineFirstScheduler(std::vector<Task *> &tasks)
            : PriorityBasedScheduler(tasks) {
            PriorityBasedScheduler::assign_priorities();
        }

    private:
        bool compare(PeriodicTask *P1, PeriodicTask *P2) override;
    };

    class LeastLaxityFirstScheduler : public PriorityBasedScheduler {
    public:
        explicit LeastLaxityFirstScheduler(std::vector<Task *> &tasks)
            : PriorityBasedScheduler(tasks) {
            PriorityBasedScheduler::assign_priorities();
        }

    private:
        bool compare(PeriodicTask *P1, PeriodicTask *P2) override;
    };

    using RM = RateMonotonicScheduler;
    using DM = DeadlineMonotonicScheduler;
    using EDF = EarliestDeadlineFirstScheduler;
    using LLF = LeastLaxityFirstScheduler;
}

#endif
