# RTSS — Real-Time Systems Software (Emulator)

A small C++ project that emulates real-time scheduling algorithms. The program accepts task descriptions from CSV files or terminal input and can run scheduling algorithms (both static and dynamic). The repository contains the emulator core, multiple scheduler implementations, utilities for reading task sets, and some unit tests (GoogleTest).

---

## Project overview

The emulator provides:

- A representation for tasks (periodic/aperiodic).
- Multiple scheduling algorithms:
  - **Static Schedulers**
    - Table-driven (use a pre-computed task-by-task schedule table)
    - Cyclic executive (frame-based)
  - **Dynamic Schedulers**
    - Deadline Monotonic (lowest relative deadline => highest priority)
    - Rate Monotonic (lowest period => highest priority)
    - Earliest Deadline First (lowest absolute deadline => highest priority)
    - Least Laxity First (lowest laxity => highest priority)
- I/O helpers that can read tasks from CSV files or write terminal input to CSV.
- GoogleTest-based unit tests.

---

## Repository layout (high level)

- `include/` — public headers (rtss namespace)
- `src/` — implementation files
  - `io/` — CSV parsing and input helpers
  - `schedulers/` — concrete scheduler implementations
- `lib/` - precompiled rtss library archive
- `tests/` — unit tests (Google Test)
- `CMakeLists.txt` — build configuration

---

## Input format

The emulator expects at least one CSV file: *task list*

- type - 'P' for periodic, 'A' for aperiodic
- phase - job release time
- period
- wcet - worst-case execution time
- rel_dl - relative deadline

Example CSV:

```csv
type,phase,period,wcet,rel_dl
P,0,5,1,5
A,1,0,5,0
P,0,10,2,10
P,0,6,1,6
P,0,15,3,15
```
---

Static scheduler also require a *schedule table*, where
- time - start time of the given task (starting with zero)
- task_id (task's order in the task list; 0 for idle, -1 for clock reset)

Notes:
- To input via terminal, use the same format as in CSV, but replace commas with spaces.

## Build

These steps assume you have CMake and a C++17 toolchain installed.

1. Create a build directory and run CMake:

```bash
mkdir -p build && cd build
cmake ..
```

2. Build and run the emulator:

```bash
cmake --build . --target rtss_emu
./rtss_emu
```

3. Build and run tests:

```bash
# $PWD='build'
cmake --build . --target run_tests
./run_tests
```

Notes:
- You should have GoogleTest installed under `/usr/local` for CMake to detect it.

---
