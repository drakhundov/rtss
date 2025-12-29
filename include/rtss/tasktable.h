#ifndef RTSS_TASKTBL_H
#define RTSS_TASKTBL_H

#include <string>

#include "rtss/task.h"
#include "rtss/time.h"
#include "rtss/frame.h"
#include "rtss/schedulers/static.h"

namespace rtss {
    struct TaskScheduleEntry {
        TaskScheduleEntry() = default;

        TaskScheduleEntry(time::TimeDuration start_time, int16_t id)
            : start_time(start_time), task_id(id) {
        }

        time::TimeDuration start_time{time::createTimeDurationMs(0)};
        int16_t task_id{static_cast<int16_t>(TaskID::IDLE)};
    };

    class TaskTable {
    public:
        ~TaskTable() = default;

        explicit TaskTable(std::vector<TaskScheduleEntry> &&schedule)
            : _schedule(std::move(schedule)) {
            if (_schedule.empty()) {
                throw std::runtime_error("[TaskTable::TaskTable] Schedule is empty");
            }
        }

        explicit TaskTable(FrameContainer *frame_container, time::TimeDuration frame_tm_dur)
            : _frame_container(frame_container),
              _scheduling_mode(StaticSchedulingMode::FRAME_BASED), _frame_tm_dur(frame_tm_dur) {
            if (_frame_tm_dur == time::ZERO_DURATION) {
                throw std::runtime_error("[TaskTable::TaskTable] Frame size cannot be zero");
            }
            if (_frame_container->empty()) {
                throw std::runtime_error("[TaskTable::TaskTable] Frames are empty");
            }
        }

        [[nodiscard]] const TaskScheduleEntry &get_kth_entry(size_t k) const {
            if (k >= _schedule.size()) {
                throw std::out_of_range("[TaskTable::get_kth_entry] Index out of range");
            }
            return _schedule[k];
        }

        [[nodiscard]] const Frame &get_kth_frame(size_t k) const {
            if (k >= _frame_container->size()) {
                throw std::out_of_range("[TaskTable::get_kth_frame] Index out of range");
            }
            // if (_scheduling_mode != StaticSchedulingMode::FRAME_BASED) {
            //     throw std::runtime_error(
            //         "[TaskTable::get_kth_frame] TaskTable should be in FRAME_BASED mode in order to retrieve frames");
            // }
            return _frame_container->get_kth_frame(k);
        }

        [[nodiscard]] const TaskScheduleEntry &get_current_entry() const {
            if (_schedule.empty()) {
                throw std::runtime_error("[TaskTable::get_current_entry] TaskTable is empty");
            }
            return _schedule[_k];
        }

        [[nodiscard]] const Frame &get_current_frame() const {
            if (_frame_container->empty()) {
                throw std::runtime_error("[TaskTable::get_current_frame] TaskTable is empty");
            }
            // if (_scheduling_mode != StaticSchedulingMode::FRAME_BASED) {
            //     throw std::runtime_error(
            //         "[TaskTable::get_current_frame] TaskTable should be in FRAME_BASED mode in order to retrieve frames");
            // }
            return _frame_container->get_kth_frame(_k);
        }

        [[nodiscard]] const TaskScheduleEntry &get_next_entry() const {
            if (_schedule.empty()) {
                throw std::runtime_error("[TaskTable::get_next_entry] TaskTable is empty");
            }
            return _schedule[(_k + 1) % _schedule.size()];
        }

        [[nodiscard]] const Frame &get_next_frame() const {
            if (_frame_container->empty()) {
                throw std::runtime_error("[TaskTable::get_current_frame] TaskTable is empty");
            }
            // if (_scheduling_mode != StaticSchedulingMode::FRAME_BASED) {
            //     throw std::runtime_error(
            //         "[TaskTable::get_current_frame] TaskTable should be in FRAME_BASED mode in order to retrieve frames");
            // }
            return _frame_container->get_kth_frame((_k + 1) % _frame_container->size());
        }

        void increment_k() noexcept {
            if (_scheduling_mode == StaticSchedulingMode::TASK_BASED) {
                _k = (_k + 1) % _schedule.size();
            } else if (_scheduling_mode == StaticSchedulingMode::FRAME_BASED) {
                _k = (_k + 1) % _frame_container->size();
            }
        }

        [[nodiscard]] size_t size() const noexcept {
            switch (_scheduling_mode) {
                case StaticSchedulingMode::TASK_BASED:
                    return _schedule.size();
                case StaticSchedulingMode::FRAME_BASED:
                    return _frame_container->size();
                default:
                    return -1;
            }
        }

        [[nodiscard]] std::string to_string() const {
            switch (_scheduling_mode) {
                case StaticSchedulingMode::TASK_BASED: {
                    std::ostringstream oss;
                    oss << "t_k\tT_k: \n";
                    for (auto schedule_entry: _schedule) {
                        oss << time::toInt(schedule_entry.start_time) << "\t" << schedule_entry.task_id << "\n";
                    }
                    return oss.str();
                }
                case StaticSchedulingMode::FRAME_BASED:
                    return _frame_container->to_string();
                default:
                    return "";
            }
        }

        [[nodiscard]] StaticSchedulingMode &scheduling_mode() const noexcept {
            return const_cast<StaticSchedulingMode &>(_scheduling_mode);
        }

        [[nodiscard]] size_t get_k() const noexcept { return _k; }

    private:
        size_t _k{0};
        const std::vector<TaskScheduleEntry> _schedule;
        const FrameContainer *const _frame_container{nullptr};
        time::TimeDuration _frame_tm_dur{time::ZERO_DURATION};
        StaticSchedulingMode _scheduling_mode{StaticSchedulingMode::TASK_BASED};
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

        // * Functions moves the schedule, so it could be built only once.
        // TODO: Enable choosing whether to move the schedule or copy it.
        TaskTable build(StaticSchedulingMode _m, time::TimeDuration frame_tm_dur = time::ZERO_DURATION,
                        const std::vector<Task *> &tasks = {}) {
            switch (_m) {
                case StaticSchedulingMode::TASK_BASED:
                    if (_schedule.empty()) {
                        throw std::runtime_error("[TaskTableBuilder::build] Schedule is empty");
                    }
                    return TaskTable(std::move(_schedule));
                case StaticSchedulingMode::FRAME_BASED: {
                    if (tasks.empty()) {
                        throw std::runtime_error("[TaskTableBuilder::build] Tasks reference is empty");
                    }
                    FrameContainer *frame_container = this->create_frames(frame_tm_dur, tasks);
                    if (frame_container == nullptr || frame_container->empty()) {
                        throw std::runtime_error("[TaskTableBuilder::build] Failed to create frames");
                    }
                    return TaskTable(frame_container, _frame_tm_dur);
                }
                default:
                    throw std::runtime_error("[TaskTableBuilder::build] Invalid StaticSchedulingMode");
            }
        }

        [[nodiscard]] size_t size() const noexcept { return _schedule.size(); }

    private:
        std::vector<TaskScheduleEntry> _schedule;
        time::TimeDuration _frame_tm_dur{time::ZERO_DURATION};

        FrameContainer *create_frames(time::TimeDuration frame_tm_dur, const std::vector<Task *> &tasks);
    };
}

#endif
