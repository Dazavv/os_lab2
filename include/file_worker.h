#ifndef OS_LAB2_FILE_WORKER_H
#define OS_LAB2_FILE_WORKER_H

#include <windows.h>
#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <vector>
#include <unordered_map>

struct FileInfo {
    off_t current_offset;  // Смещение для данного файла
};

// Хранилище данных о файлах (сопоставляем fd с FileInfo)
extern std::unordered_map<int, FileInfo> file_map;

int lab2_open(const char *path);
int lab2_close(int fd);
ssize_t lab2_read(int fd, void *buf, size_t count);
ssize_t lab2_write(int fd, const void *buf, size_t count);
off_t lab2_lseek(int fd, off_t offset, int whence);
int lab2_fsync(int fd);

#endif //OS_LAB2_FILE_WORKER_H
