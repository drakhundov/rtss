#include "rtss/io/input.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "rtss/task.h"
#include "rtss/tasktable.h"
#include "rtss/time.h"

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
                    throw std::runtime_error(
                        "[io::read_task_list_from_csv] Invalid task type: " + std::string(1, act_type));
            }
            T->set_id(id_counter++);
            tasks.push_back(T);
        }
        ifs.close();
    }

    void read_task_list_from_csv(std::vector<PeriodicTask *> &periodic, std::vector<AperiodicTask *> &aperiodic,
                                 const std::string &file_path, Metadata &meta) {
        if (!periodic.empty()) {
            throw std::runtime_error("[io::read_task_list_from_csv] std::vector<PeriodicTask> passed has to be empty");
        }
        if (!aperiodic.empty()) {
            throw std::runtime_error("[io::read_task_list_from_csv] std::vector<AperiodicTask> passed has to be empty");
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
                    periodic.emplace_back(dynamic_cast<PeriodicTask *>(T));
                    break;
                case 'A':
                    T = new AperiodicTask(time::createTimeDurationMs(phase), time::createTimeDurationMs(wcet));
                    aperiodic.emplace_back(dynamic_cast<AperiodicTask *>(T));
                    meta.fully_periodic = false;
                    break;
                default:
                    throw std::runtime_error(
                        "[io::read_task_list_from_csv] Invalid task type: " + std::string(1, act_type));
            }
            T->set_id(id_counter++);
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

    void write_task_csv_from_stdin(const std::filesystem::path &csv_path) {
        std::ofstream out(csv_path);
        if (!out) {
            throw std::runtime_error("Failed to open CSV file for writing: " + csv_path.string());
        }

        out << "type,phase,period,wcet,rel_dl\n";

        std::string line;
        while (std::getline(std::cin, line)) {
            // stop on empty line
            if (line.empty()) {
                break;
            }

            std::istringstream iss(line);
            std::string type;
            iss >> type;
            if (!iss) {
                continue; // skip malformed/empty line
            }

            if (type == "P" || type == "p") {
                std::vector<std::string> tokens;
                std::string tok;
                while (iss >> tok) {
                    tokens.push_back(tok);
                }

                if (tokens.size() != 3 && tokens.size() != 4) {
                    throw std::runtime_error("Invalid periodic task line: " + line);
                }

                int phase = 0;
                int wcet = 0;
                int period = 0;
                int rel_dl = 0;

                // P ri ei pi [di]
                // tokens: [ri, ei, pi, (di)?]
                phase = (tokens[0] == "-") ? 0 : std::stoi(tokens[0]);
                wcet = std::stoi(tokens[1]);
                period = std::stoi(tokens[2]);

                if (tokens.size() == 4) {
                    rel_dl = std::stoi(tokens[3]);
                } else {
                    rel_dl = period; // default D = T
                }

                out << "P," << phase << "," << period << "," << wcet << "," << rel_dl << "\n";
            } else if (type == "A" || type == "a") {
                std::vector<std::string> tokens;
                std::string tok;
                while (iss >> tok) {
                    tokens.push_back(tok);
                }

                if (tokens.size() != 2 && tokens.size() != 3) {
                    throw std::runtime_error("Invalid aperiodic task line: " + line);
                }

                int arrival = 0;
                int wcet = 0;
                int rel_dl = 0;

                // A ri ei [di]
                arrival = std::stoi(tokens[0]);
                wcet = std::stoi(tokens[1]);
                if (tokens.size() == 3) {
                    rel_dl = std::stoi(tokens[2]);
                } else {
                    rel_dl = 0; // no soft deadline
                }

                // period = 0 for aperiodic in the CSV schema
                out << "A," << arrival << ",0," << wcet << "," << rel_dl << "\n";
            } else {
                throw std::runtime_error("Unknown task type in line: " + line);
            }
        }
    }

    void write_task_table_csv_from_stdin(const std::filesystem::path &csv_path) {
        std::ofstream out(csv_path);
        if (!out) {
            throw std::runtime_error("Failed to open table CSV file for writing: " + csv_path.string());
        }

        out << "time,task_id\n";

        std::string line;
        while (true) {
            std::cout << "> ";
            if (!std::getline(std::cin, line)) break;        // EOF
            if (line.empty()) break;                         // user ended input

            std::istringstream iss(line);
            int time_ms = 0;
            int task_id = 0;

            if (!(iss >> time_ms >> task_id)) {
                std::cerr << "Invalid line, expected: <time> <task_id>\n";
                continue;
            }

            out << time_ms << "," << task_id << "\n";
        }
    }
}
