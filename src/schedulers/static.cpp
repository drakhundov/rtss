#include "rtss/schedulers/static.h"

#include <iostream>

namespace rtss::schedulers {
    void TableDrivenScheduler::run_scheduler(size_t nperiods) {
        TaskScheduleEntry se;
        Task *T;
        size_t period_counter = 0;
        int16_t task_id;
        se = this->task_tbl.get_current_entry();
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
                std::cout << "T" << T->get_id() << " duration = " << rtss::time::toInt(exec_time) << " time::createTimeDurationMs" << std::endl;
                this->task_tbl.increment_k();
                se = this->task_tbl.get_current_entry();
            }
            std::cout << "---- End of hyperperiod ----" << std::endl;
            // End of the hyperperiod, reset the tasks.
            for (auto task: this->tasks) { task->reset(); }
            period_counter++;
        }
    }

    void CyclicExecutiveScheduler::run_scheduler(size_t nperiods) {
        size_t period_counter = 0;
        while (period_counter < nperiods) {
            std::cout << "---- Hyperperiod " << period_counter + 1 << " ----" << std::endl;
            do {
                const auto &frame = this->task_tbl.get_current_frame();
                frame.run_frame(this->tasks);
                this->task_tbl.increment_k();
            } while (this->task_tbl.get_k() != 0);
            // End of the hyperperiod, reset the tasks.
            for (auto task: this->tasks) { task->reset(); }
            period_counter++;
        }
    }
}
