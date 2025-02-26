#include <gtest/gtest.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <cstdio>

#include "../../include/file_worker.h"
#include "../../include/cache.h"
#include "../../include/io_lat_write.h"


TEST(IOTest, CachePerformance) {
    std::string file_path = "test_file_io.txt";
    int iterations = 1000;

    std::ofstream test_file(file_path);
    test_file.close();

    auto start_time_no_cache = std::chrono::high_resolution_clock::now();
    IOLatWrite(iterations, file_path);
    auto end_time_no_cache = std::chrono::high_resolution_clock::now();
    auto elapsed_time_no_cache = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_no_cache - start_time_no_cache).count();

    auto start_time_with_cache = std::chrono::high_resolution_clock::now();
    IOLatWriteWithCash(iterations, file_path);
    auto end_time_with_cache = std::chrono::high_resolution_clock::now();
    auto elapsed_time_with_cache = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_with_cache - start_time_with_cache).count();

    std::cout << "Time without cache: " << elapsed_time_no_cache << " ms" << std::endl;
    std::cout << "Time with cache: " << elapsed_time_with_cache << " ms" << std::endl;

    std::remove(file_path.c_str());
    ASSERT_LT(elapsed_time_with_cache, elapsed_time_no_cache);
}

