#include "rtss/schedulers/clock-based.h"

#include <iostream>
#include <algorithm>

namespace rtss::schedulers {
    void TableDrivenScheduler::run_scheduler(size_t nperiods) {
        ScheduleEntry se = this->task_tbl.get_current_entry();
        Task *T;
        size_t period_counter = 0;
        int16_t task_id;
        while (period_counter < nperiods) {
            while (se.task_id != static_cast<int16_t>(TaskID::RESET)) {
                task_id = se.task_id;
                if (task_id == static_cast<int16_t>(TaskID::IDLE)) {
                    T = Task::Idle();
                } else {
                    T = this->tasks[task_id - 1]; // task_id starts from 1 so that 0 and -1 can be reserved.
                }
                time::TimeDuration exec_time = this->task_tbl.get_next_entry().start_time - se.start_time;
                T->run_task(exec_time);
                std::cout << "T" << T->get_id() << " duration = " << rtss::time::toInt(exec_time) << " ms" << std::endl;
                this->task_tbl.increment_k();
                se = this->task_tbl.get_current_entry();
            }
            std::cout << "---- End of hyperperiod ----" << std::endl;
            // End of the hyperperiod, reset the tasks.
            for (auto task: this->tasks) { task->reset(); }
            period_counter++;
        }
    }
}
