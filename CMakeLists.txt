cmake_minimum_required(VERSION 3.30)
project(os_lab2)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG        release-1.11.0
)

FetchContent_MakeAvailable(googletest)

add_library(cache SHARED src/cache.cpp)

add_executable(tests
        src/tests/test_cache.cpp
        src/tests/test_file_worker.cpp
        src/tests/test_io_lat_write.cpp
        src/io_lat_write_w_cash.cpp
        src/io_lat_write.cpp
        src/file_worker.cpp
        src/tools.cpp)

add_executable(io_lat_write_w_cash
        src/io_lat_write_w_cash.cpp
        src/file_worker.cpp
        src/cache.cpp
        src/tools.cpp

)
add_executable(test_c
        src/tests/test.cpp
        src/file_worker.cpp
        src/cache.cpp
        src/tools.cpp

)
add_executable(io_lat_write
        src/io_lat_write.cpp
)

target_link_libraries(tests
        PRIVATE cache
        gtest gtest_main)

target_compile_definitions(tests PRIVATE TESTING)

#set_target_properties(cache PROPERTIES OUTPUT_NAME "cache")
include_directories(${googletest_SOURCE_DIR}/googletest/include)

