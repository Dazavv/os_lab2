#ifndef OS_LAB2_IO_LAT_WRITE_H
#define OS_LAB2_IO_LAT_WRITE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <string>
#include <filesystem>
#include <windows.h>

void IOLatWrite(int iterations, const std::string& filePath);
void IOLatWriteWithCash(int iterations, const std::string& filePath);


#endif //OS_LAB2_IO_LAT_WRITE_H
