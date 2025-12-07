#pragma once

#include <string>

#include "rtss/task.h"
#include "rtss/time.h"

namespace rtss {
    struct ScheduleEntry {
        ScheduleEntry(time::TimeDuration start_time, int16_t id)
            : start_time(start_time), task_id(id) {
            ;
        }

        time::TimeDuration start_time{time::createTimeDurationMs(0)};
        int16_t task_id{0};
    };

    class TaskTable {
    public:
        TaskTable() = default;

        ~TaskTable() = default;

        explicit TaskTable(std::vector<ScheduleEntry> &&schedule)
            : _schedule(std::move(schedule)), _k(0) {
        }

        [[nodiscard]] const ScheduleEntry &get_kth_entry(size_t k) const {
            if (k >= _schedule.size()) {
                throw std::out_of_range("[TaskTable::get_kth_entry] Index out of range");
            }
            return _schedule[k];
        }

        [[nodiscard]] const ScheduleEntry &get_current_entry() const {
            if (_schedule.empty()) {
                throw std::runtime_error("[TaskTable::get_current_entry] TaskTable is empty");
            }
            return _schedule[_k];
        }

        void increment_k() noexcept {
            _k = (_k + 1) % _schedule.size();
        }

        [[nodiscard]] size_t size() const noexcept {
            return _schedule.size();
        }

        [[nodiscard]] std::string to_string() const {
            std::ostringstream oss;
            oss << "t_k\tT_k: \n";
            for (auto schedule_entry : _schedule) {
                oss << time::toInt(schedule_entry.start_time) << "\t" << schedule_entry.task_id << "\n";
            }
            return oss.str();
        }

    private:
        const std::vector<ScheduleEntry> _schedule;
        size_t _k{0};
    };

    class TaskTableBuilder {
    public:
        void add_entry(const Task &T, time::TimeDuration start_time) {
            auto id = (int16_t) T.get_id();
            _schedule.emplace_back(start_time, id);
        }

        void add_entry(int16_t task_id, time::TimeDuration start_time) {
            _schedule.emplace_back(start_time, task_id);
        }

        TaskTable build() { return TaskTable(std::move(_schedule)); }

        [[nodiscard]] size_t size() const noexcept {
            return _schedule.size();
        }

    private:
        std::vector<ScheduleEntry> _schedule;
    };
}
