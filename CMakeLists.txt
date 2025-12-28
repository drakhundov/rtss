cmake_minimum_required(VERSION 3.15)
project(rtss LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(rtss_lib
        src/schedulers/static.cpp
        src/schedulers/dynamic.cpp
        src/io/input.cpp
        src/frame.cpp
        src/tasktable.cpp
)

add_executable(rtss_emu
        src/main.cpp
)

target_link_libraries(rtss_emu
        PRIVATE
        rtss_lib
)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests)
