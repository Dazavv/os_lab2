#include "../include/io_lat_write.h"

void IOLatWrite(int iterations, const std::string& filePath) {
    const int block_size = 1024; // размер блока 1К
    std::vector<char> data(block_size, 'A');

    HANDLE file = CreateFileA(filePath.c_str(),
                              GENERIC_WRITE,
                              0,
                              NULL,
                              OPEN_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL);
    if (file == INVALID_HANDLE_VALUE) {
        std::cerr << "error opening file: " << filePath<< std::endl;
        return;
    }

    // Генератор случайных чисел для выбора случайных смещений в файле
    std::random_device rd;
    std::mt19937 gen(rd());
    // Распределение для выбора смещения от 0 до 1023
    std::uniform_int_distribution<> dis(0, 1023);

    auto start_time = std::chrono::high_resolution_clock::now();

    // Цикл записи данных в файл заданное количество раз
    for (int i = 0; i < iterations; ++i) {
        int offset = dis(gen); // Генерируем случайное смещение
        // Устанавливаем указатель записи в файл на сгенерированное смещение
        if (SetFilePointer(file, offset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
            std::cerr << "Error seeking in file " << std::endl;
            CloseHandle(file);
            return;
        }

        DWORD bytesWritten;
        if (!WriteFile(file, data.data(), block_size, &bytesWritten, NULL) || bytesWritten != block_size) {
            std::cerr << "Error writing to file" << std::endl;
            return;
        }
        if (!FlushFileBuffers(file)) {
            std::cerr << "Error flushing buffers" << std::endl;
        }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "average write time per iteration: " << static_cast<double>(elapsed_time) / iterations << " [ms]" << std::endl;

    CloseHandle(file);
}
#ifndef TESTING
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: io-lat-write <outputFile> <number of iterations>" << std::endl;
        return 1;
    }

    int iterations = std::stoi(argv[2]);
    IOLatWrite(iterations, argv[1]);
    return 0;
}
#endif