#include "rtss/io/input.h"

#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "rtss/task.h"
#include "rtss/time.h"


namespace rtss::io {
    std::ifstream _init_ifs_for_csv(const std::string &file_path, const std::string &header) {
        std::ifstream ifs(file_path);
        if (!ifs) {
            throw std::runtime_error("Failed to open " + file_path);
        }
        std::string line;
        std::getline(ifs, line);
        if (line != header) {
            throw std::runtime_error("Invalid CSV header: " + line);
        }
        return ifs;
    }

    void read_task_list_from_csv(std::vector<Task *> &tasks, const std::string &file_path, Metadata &meta) {
        if (!tasks.empty()) {
            throw std::runtime_error("std::vector<Task> passed has to be empty");
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
                throw std::runtime_error("Invalid CSV line: " + line);
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
                    throw std::runtime_error("Invalid task type: " + std::string(1, act_type));
            }
            T->set_id(id_counter++);
            tasks.push_back(T);
        }
        ifs.close();
    }
}
