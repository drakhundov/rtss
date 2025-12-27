cmake_minimum_required(VERSION 3.15)
project(rtss LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(rtss_lib
        src/schedulers/clock-based.cpp
        src/io/input.cpp
)

add_executable(rtss_app
        src/main.cpp
)

target_link_libraries(rtss_app
        PRIVATE
        rtss_lib
)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests)
