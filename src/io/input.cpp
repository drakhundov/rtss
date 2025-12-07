#include "rtss/io/input.h"

#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "rtss/task.h"
#include "rtss/time.h"

enum class TaskID {
    RESET = -1,
    IDLE = 0
};


namespace rtss::io {
    std::ifstream _init_ifs_for_csv(const std::string &file_path, const std::string &header) {
        std::ifstream ifs(file_path);
        if (!ifs) {
            throw std::runtime_error("[io::_init_ifs_for_csv] Failed to open " + file_path);
        }
        std::string line;
        std::getline(ifs, line);
        if (line != header) {
            throw std::runtime_error("[io::_init_ifs_for_csv] Invalid CSV header: " + line);
        }
        return ifs;
    }

    void read_task_list_from_csv(std::vector<Task *> &tasks, const std::string &file_path, Metadata &meta) {
        if (!tasks.empty()) {
            throw std::runtime_error("[io::read_task_list_from_csv] std::vector<Task> passed has to be empty");
        }
        std::ifstream ifs = _init_ifs_for_csv(file_path, "type,phase,period,wcet,rel_dl");
        std::string line;
        meta.fully_periodic = true;
        Task *T; // Temporary pointer for task creation.
        short id_counter = 1;
        while (std::getline(ifs, line)) {
            if (line.empty() || line[0] == '#')
                continue;
            std::istringstream iss(line);
            char act_type, comma;
            int phase, period, wcet, rel_dl;
            if (!(iss >> act_type >> comma >> phase >> comma >> period >> comma >> wcet >> comma >> rel_dl)) {
                throw std::runtime_error("[io::read_task_list_from_csv] Invalid CSV line: " + line);
            }
            switch (act_type) {
                case 'P':
                    T = new PeriodicTask(time::createTimeDurationMs(phase), time::createTimeDurationMs(period),
                                         time::createTimeDurationMs(wcet), time::createTimeDurationMs(rel_dl));
                    break;
                case 'A':
                    T = new AperiodicTask(time::createTimeDurationMs(phase), time::createTimeDurationMs(wcet));
                    meta.fully_periodic = false;
                    break;
                default:
                    throw std::runtime_error("[io::read_task_list_from_csv] Invalid task type: " + std::string(1, act_type));
            }
            T->set_id(id_counter++);
            tasks.push_back(T);
        }
        ifs.close();
    }

    void read_task_table_from_csv(TaskTableBuilder &tbl_builder, const std::string &file_path) {
        if (tbl_builder.size() != 0) {
            throw std::runtime_error("[io::read_task_table_from_csv] Passed non-empty task table builder to parser");
        }
        std::ifstream ifs = _init_ifs_for_csv(file_path, "time,task_id");
        std::string line;
        while (std::getline(ifs, line)) {
            if (line.empty() || line[0] == '#')
                continue;
            std::istringstream iss(line);
            int time_ms;
            int16_t task_id;
            char comma;
            if (!(iss >> time_ms >> comma >> task_id)) {
                throw std::runtime_error("[io::read_task_table_from_csv] Invalid CSV line: " + line);
            }
            time::TimeDuration start_time = time::createTimeDurationMs(time_ms);
            if (task_id == static_cast<int>(TaskID::IDLE)) {
                tbl_builder.add_entry(static_cast<int16_t>(TaskID::IDLE), start_time);
            } else if (task_id == static_cast<int>(TaskID::RESET)) {
                tbl_builder.add_entry(static_cast<int16_t>(TaskID::RESET), start_time);
            } else if (task_id > 0) {
                tbl_builder.add_entry(task_id, start_time);
            } else {
                throw std::runtime_error("[io::read_task_table_from_csv] Invalid task ID: " + std::to_string(task_id));
            }
        }
    }
}
