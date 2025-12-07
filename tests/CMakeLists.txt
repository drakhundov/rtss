enable_testing()

set(GTEST_ROOT "/usr/local")
set(GTEST_INCLUDE_DIR "/usr/local/include")
set(GTEST_LIBRARY "/usr/local/lib/libgtest.a")
set(GTEST_MAIN_LIBRARY "/usr/local/lib/libgtest_main.a")

add_executable(run_tests
        test_time.cpp
        test_task_impl.cpp
        test_read_csv.cpp
)

target_link_libraries(run_tests
        PRIVATE
        /usr/local/lib/libgtest.a
        /usr/local/lib/libgtest_main.a
        rtss_lib
)

add_test(NAME rtss_tests COMMAND run_tests)
