#ifndef RTSS__TASK_H
#define RTSS__TASK_H

#include <string>
#include <sstream>

#include "rtss/time.h"

namespace rtss {
    class Task {
    public:
        Task() = default;

        virtual ~Task() = default;

        Task(time::TimeDuration phase, time::TimeDuration wcet) noexcept
            : _phase(phase), _wcet(wcet) {
        }

        [[nodiscard]] time::TimeDuration get_phase() const noexcept { return _phase; }

        [[nodiscard]] time::TimeDuration get_wcet() const noexcept { return _wcet; }

        [[nodiscard]] short get_id() const noexcept { return _id; }

        void set_phase(const time::TimeDuration &phase) noexcept {
            _phase = phase;
        }

        void set_wcet(const time::TimeDuration &wcet) noexcept {
            _wcet = wcet;
        }

        void set_id(short id) {
            // !Set id only once when creating the task.
            if (_id == 0) {
                _id = id;
            } else {
                throw std::runtime_error("id could only be set once");
            }
        }

        [[nodiscard]] bool is_idle() const noexcept {
            return _wcet == time::TimeDuration::zero();
        }

        static Task *Idle() {
            if (_idle == nullptr) {
                _idle = new Task(time::TimeDuration::zero(), time::TimeDuration::zero());
            }
            return _idle;
        }

        [[nodiscard]] virtual std::string to_string() const {
            std::ostringstream oss;
            oss << "phase = " << time::toInt(_phase)
                    << " wcet = " << time::toInt(_wcet);
            return oss.str();
        }

    private:
        time::TimeDuration _phase{time::ZERO_DURATION}, _wcet{time::ZERO_DURATION};
        uint16_t _id{0};
        static Task *_idle;
    };

    class PeriodicTask : public Task {
    public:
        PeriodicTask()
            : Task(), _period(time::TimeDuration::zero()),
              _rel_dl(time::TimeDuration::zero()) {
        }

        PeriodicTask(time::TimeDuration phase, time::TimeDuration period, time::TimeDuration wcet,
                     time::TimeDuration rel_dl) noexcept
            : Task(phase, wcet),
              _period(period), _rel_dl(rel_dl) {
        }

        PeriodicTask(time::TimeDuration period, time::TimeDuration wcet)
            : Task(time::TimeDuration::zero(), wcet),
              _period(period), _rel_dl(period) {
        }

        PeriodicTask(time::TimeDuration period, time::TimeDuration wcet, time::TimeDuration rel_dl) noexcept
            : Task(time::TimeDuration::zero(), wcet),
              _period(period), _rel_dl(rel_dl) {
        }

        [[nodiscard]] time::TimeDuration get_period() const noexcept {
            return _period;
        }

        [[nodiscard]] time::TimeDuration get_rel_dl() const noexcept {
            return _rel_dl;
        }

        void set_period(const time::TimeDuration &period) noexcept {
            _period = period;
        }

        void set_rel_dl(const time::TimeDuration &rel_dl) noexcept {
            _rel_dl = rel_dl;
        }

        [[nodiscard]] std::string to_string() const override {
            std::ostringstream oss;
            oss << "phase = " << time::toInt(get_phase())
                    << " period = " << time::toInt(_period)
                    << " wcet = " << time::toInt(get_wcet())
                    << " rel_dl = " << time::toInt(_rel_dl);
            return oss.str();
        }

    private:
        time::TimeDuration _period{time::ZERO_DURATION}, _rel_dl{time::ZERO_DURATION};
    };

    class AperiodicTask : public Task {
    public:
        AperiodicTask() = default;

        AperiodicTask(time::TimeDuration arrival, time::TimeDuration wcet) noexcept
            : Task(arrival, wcet) {
        }

        [[nodiscard]] time::TimeDuration get_arrival() const noexcept { return get_phase(); }
        void set_arrival(time::TimeDuration arrival) noexcept { set_phase(arrival); }

        [[nodiscard]] std::string to_string() const override {
            std::ostringstream oss;
            oss << "arrival = " << time::toInt(get_arrival())
                    << " wcet = " << time::toInt(get_wcet());
            return oss.str();
        }
    };
}

#endif
