#include "../include/io_lat_write.h"
#include "../include/cache.h"
#include "../include/file_worker.h"
#include "../include/tools.h"

void IOLatWriteWithCash(int iterations, const std::string& filePath) {
    const int block_size = 1024; // размер блока 1К
    std::vector<char> data(block_size, 'A');

    int fd = lab2_open(filePath.c_str());
    if (fd == -1) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return;
    }

    // Генератор случайных чисел для выбора случайных смещений в файле
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1023);  // Генерация случайных смещений

    auto start_time = std::chrono::high_resolution_clock::now();

    // Цикл записи данных в файл заданное количество раз
    for (int i = 0; i < iterations; ++i) {
        int offset = dis(gen);  // Генерация случайного смещения

        lab2_lseek(fd, offset, SEEK_SET);
        lab2_write(fd, data.data(), block_size);
    }
    lab2_fsync(fd);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "average write time per iteration: " << static_cast<double>(elapsed_time) / iterations << " [ms]" << std::endl;

    lab2_close(fd);
}

#ifndef TESTING
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: io-lat-write <outputFile> <number of iterations>" << std::endl;
        return 1;
    }

    int iterations = std::stoi(argv[2]);
    IOLatWriteWithCash(iterations, argv[1]);
    return 0;
}
#endif
