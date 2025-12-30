#include "rtss/schedulers/dynamic.h"

#include <numeric>
#include <iostream>

namespace rtss::schedulers {
    void PriorityBasedScheduler::assign_priorities(std::vector<size_t> &idx) {
        const size_t n = this->tasks.size();
        if (n == 0) return;

        // sort indices so original `tasks` order is preserved
        std::iota(idx.begin(), idx.end(), 0);

        std::sort(idx.begin(), idx.end(),
                  [this](size_t ia, size_t ib) -> bool {
                      Task *a = this->tasks[ia];
                      Task *b = this->tasks[ib];

                      auto pa = dynamic_cast<PeriodicTask *>(a);
                      auto pb = dynamic_cast<PeriodicTask *>(b);

                      if (pa && pb) {
                          return this->compare(pa, pb);
                      }
                      if (pa && !pb) {
                          return true; // periodic tasks before non-periodic
                      }
                      if (!pa && pb) {
                          return false;
                      }
                      // fallback: compare WCET
                      return a->get_wcet() < b->get_wcet();
                  });

        // // assign priorities (1 = highest here)
        // for (size_t rank = 0; rank < n; ++rank) {
        //     set_task_priority(idx[rank], static_cast<int>(rank + 1));
        // }
    }

    void PriorityBasedScheduler::run_scheduler(size_t ncycles) {
        if (ncycles == 0) return;

        size_t cycle_counter = 0;
        if (this->_priority_mode == PriorityMode::FIXED) {
            assign_priorities(this->pri_idx);
            std::cout << "Assigned priorities." << std::endl;
        }
        while (cycle_counter < ncycles) {
            std::cout << "---- Cycle " << cycle_counter + 1 << " ----" << std::endl;
            if (this->_priority_mode == PriorityMode::DYNAMIC) {
                assign_priorities(this->pri_idx);
                std::cout << "Assigned priorities." << std::endl;
            }
            bool progress = true;
            while (progress) {
                progress = false;
                auto now = time::Clock::now().time_since_epoch();

                for (size_t idx: pri_idx) {
                    Task *t = this->tasks[idx];

                    if (t->get_rem_tm() == time::TimeDuration::zero()) continue; // already finished

                    bool ready = false;
                    if (auto *pt = dynamic_cast<PeriodicTask *>(t)) {
                        // treat task as available if current time >= phase (first release)
                        ready = (now >= pt->get_phase());
                    } else if (auto *at = dynamic_cast<AperiodicTask *>(t)) {
                        ready = (now >= at->get_arrival());
                    } else {
                        ready = (now >= t->get_phase());
                    }

                    if (!ready) continue;

                    // run the task to completion (consume remaining time)
                    time::TimeDuration exec = t->get_rem_tm();
                    std::cout << "Running T" << t->get_id() << " for " << time::toInt(exec) << "ms" << std::endl;
                    t->run_task(exec);
                    std::cout << "T" << t->get_id() << " remaining=" << time::toInt(t->get_rem_tm()) << "ms" <<
                            std::endl;

                    progress = true;
                    // after running one job, re-evaluate readiness/priorities in the next loop iteration
                    break;
                }
            }
            for (auto *t: this->tasks) {
                t->reset();
            }
            cycle_counter++;
        }
    }

    bool RateMonotonicScheduler::compare(PeriodicTask *P1, PeriodicTask *P2) {
        return P1->get_period() < P2->get_period();
    }

    bool DeadlineMonotonicScheduler::compare(PeriodicTask *P1, PeriodicTask *P2) {
        return P1->get_rel_dl() < P2->get_rel_dl();
    }

    bool EarliestDeadlineFirstScheduler::compare(PeriodicTask *P1, PeriodicTask *P2) {
        return P1->calc_abs_dl() < P2->calc_abs_dl();
    }

    bool LeastLaxityFirstScheduler::compare(PeriodicTask *P1, PeriodicTask *P2) {
        return P1->calc_laxity() < P2->calc_laxity();
    }
}
