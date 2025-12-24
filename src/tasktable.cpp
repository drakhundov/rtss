#include "rtss/tasktable.h"

#include "rtss/time.h"
#include "rtss/frame.h"

namespace rtss {
    FrameContainer *TaskTableBuilder::create_frames(time::TimeDuration frame_tm_dur,
                                                    const std::vector<Task *> &tasks) {
        this->_frame_tm_dur = frame_tm_dur;
        if (_schedule.empty()) {
            throw std::runtime_error("[TaskTableBuilder::create_frames] Schedule is empty");
        }
        if (frame_tm_dur == time::ZERO_DURATION) {
            throw std::runtime_error("[TaskTableBuilder::create_frames] Frame duration is zero");
        }
        std::vector<Frame> frames;
        std::vector<FrameJob> current_jobs;
        TaskScheduleEntry se;
        time::TimeDuration cur_frame_dur = time::ZERO_DURATION, task_dur, rem_tm_dur= time::ZERO_DURATION;
        auto flush_frame = [&]() {
            if (!current_jobs.empty()) {
                frames.emplace_back(std::move(current_jobs));
                current_jobs.clear();
                cur_frame_dur = time::ZERO_DURATION;
            }
        };
        for (size_t i = 0; i + 1 < _schedule.size(); i++) {
            se = _schedule[i];
            task_dur = _schedule[i + 1].start_time - _schedule[i].start_time;
            cur_frame_dur += task_dur;
            if (cur_frame_dur > frame_tm_dur) {
                // Need to split the task across frames.
                time::TimeDuration time_to_fill = frame_tm_dur - (cur_frame_dur - task_dur);
                current_jobs.emplace_back(se.task_id, time_to_fill);
                flush_frame();
                // Now handle the remaining part of the task.
                time::TimeDuration rem_task_dur = task_dur - time_to_fill;
                while (rem_task_dur > time::ZERO_DURATION) {
                    if (rem_task_dur >= frame_tm_dur) {
                        current_jobs.emplace_back(se.task_id, frame_tm_dur);
                        flush_frame();
                        rem_task_dur -= frame_tm_dur;
                    } else {
                        current_jobs.emplace_back(se.task_id, rem_task_dur);
                        cur_frame_dur = rem_task_dur;
                        rem_task_dur = time::ZERO_DURATION;
                    }
                }
            } else if (cur_frame_dur == frame_tm_dur) {
                current_jobs.emplace_back(se.task_id, task_dur);
                flush_frame();
            } else {
                current_jobs.emplace_back(se.task_id, task_dur);
            }
            rem_tm_dur = (cur_frame_dur + task_dur) % frame_tm_dur;
        }
        return new FrameContainer(tasks, std::move(frames), frame_tm_dur);
    }
}
