#ifndef RTSS_FRAME_H
#define RTSS_FRAME_H

#include <vector>

#include "rtss/task.h"

namespace rtss {
    struct FrameJob {
        FrameJob(int16_t task_id, time::TimeDuration exec_tm)
            : task_id(task_id), exec_tm(exec_tm) {
        }

        int16_t task_id;
        time::TimeDuration exec_tm;
    };

    class Frame {
    public:
        explicit Frame(const std::vector<Task *> &tasks_ref)
            : _tasks_ref(const_cast<std::vector<Task *> &>(tasks_ref)) {
        }

        void run_frame();

        void add_job(int16_t task_id, time::TimeDuration exec_tm) {
            _jobs.emplace_back(task_id, exec_tm);
        }

    private:
        // Each frame would remember which particular tasks it needs to run.
        const std::vector<Task *> &_tasks_ref;
        std::vector<FrameJob> _jobs;
    };
}

#endif
