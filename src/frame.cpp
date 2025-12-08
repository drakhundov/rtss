#include "rtss/frame.h"

#include <iostream>

#include "rtss/task.h"

namespace rtss {
    void Frame::run_frame() {
        if (_jobs.empty()) {
            throw std::runtime_error("[Frame::run_frame] No jobs to run in this frame.");
        }
        for (const auto &job: _jobs) {
            Task *T;
            if (job.task_id == static_cast<int16_t>(TaskID::IDLE)) {
                T = Task::Idle();
            } else {
                T = _tasks_ref[job.task_id - 1]; // task_id starts from 1 so that 0 and -1 can be reserved.
            }
            T->run_task(job.exec_tm);
            std::cout << "\tT" << T->get_id() << " duration = " << rtss::time::toInt(job.exec_tm) << " ms" << std::endl;
        }
    }
}
