#ifndef RTSS_FRAME_H
#define RTSS_FRAME_H

#include <vector>

#include "rtss/task.h"

namespace rtss {
    struct FrameJob {
        FrameJob(int16_t task_id, time::TimeDuration exec_tm)
            : task_id(task_id), exec_tm(exec_tm) {
        }

        [[nodiscard]] std::string to_string() const {
            std::ostringstream oss;
            oss << "{T" << task_id << ", " << time::toInt(exec_tm) << "ms}";
            return oss.str();
        }

        int16_t task_id;
        time::TimeDuration exec_tm;
    };

    class Frame {
    public:
        explicit Frame(const std::vector<FrameJob> &&jobs)
            : _jobs(jobs) {
        }

        void run_frame(const std::vector<Task *> &tasks_ref) const;

        [[nodiscard]] std::string to_string() const {
            std::string result;
            for (auto &fj: _jobs) {
                result += fj.to_string() + "\n";
            }
            return result;
        }

    private:
        // Each frame would remember which particular tasks it needs to run.
        const std::vector<FrameJob> _jobs;
    };

    class FrameContainer {
    public:
        explicit FrameContainer(const std::vector<Task *> &tasks_ref, const std::vector<Frame> &&frames,
                                time::TimeDuration frame_tm_dur)
            : _frames(frames), _tasks_ref(const_cast<std::vector<Task *> &>(tasks_ref)),
              _frame_tm_dur(frame_tm_dur) {
        }

        void run_frame(size_t k) {
            if (k >= _frames.size()) {
                throw std::out_of_range("[FrameContainer::run_frame] Index out of range");
            }
            _frames[k].run_frame(_tasks_ref);
        }

        [[nodiscard]] std::string to_string() const {
            std::string result;
            for (size_t i = 0; i < _frames.size(); ++i) {
                result += "Frame " + std::to_string(i) + ":\n";
                result += _frames[i].to_string() + "\n";
            }
            return result;
        }

        [[nodiscard]] const Frame &get_kth_frame(size_t k) const {
            if (k >= _frames.size()) {
                throw std::out_of_range("[FrameContainer::get_kth_frame] Index out of range");
            }
            return _frames[k];
        }

        [[nodiscard]] size_t size() const noexcept { return _frames.size(); }
        [[nodiscard]] bool empty() const noexcept { return _frames.empty(); }

    private:
        const std::vector<Task *> &_tasks_ref;
        const std::vector<Frame> _frames;
        time::TimeDuration _frame_tm_dur{time::ZERO_DURATION};
    };
}

#endif
