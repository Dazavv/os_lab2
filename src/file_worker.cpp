#include "../include/file_worker.h"
#include "../include/cache.h"

std::unordered_map<int, FileInfo> file_map;

// Открытие файла по заданному пути файла, доступного для чтения
int lab2_open(const char *path) {
    HANDLE file = CreateFileA(path,
                              GENERIC_READ | GENERIC_WRITE,
                              FILE_SHARE_READ,
                              NULL,
                              OPEN_EXISTING,
                              FILE_FLAG_NO_BUFFERING,
                              NULL);
    if (file == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        std::cerr << "Error opening file: " << error << std::endl;
        return -1;
    }

    int fd = ::_open_osfhandle((intptr_t) file, _O_RDWR);
    file_map[fd] = {0};
    return fd;
}

// Закрытие файла по хэндлу
int lab2_close(int fd) {
    cache_flush(fd);
    file_map.erase(fd);

    HANDLE file = (HANDLE) _get_osfhandle(fd);
    if (file == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: invalid file descriptor" << std::endl;
        return -1;
    }

    if (_close(fd) == -1) {
        std::cerr << "Error closing file descriptor" << std::endl;
        return -1;
    }
    return 0;
}

// Чтение данных из файла
ssize_t lab2_read(int fd, void *buf, size_t count) {
    off_t offset = file_map[fd].current_offset;
    cache_read(fd, offset, (char *) buf, count);
    file_map[fd].current_offset += count;
    return count;
}

// Запись данных в файл
ssize_t lab2_write(int fd, const void *buf, size_t count) {
    off_t offset = file_map[fd].current_offset;
    cache_write(fd, offset, (const char *) buf, count);
    file_map[fd].current_offset += count;
    return count;
}

// Перестановка позиции указателя на данные файла
off_t lab2_lseek(int fd, off_t offset, int whence) {
    if (whence != SEEK_SET) {
        std::cerr << "Error: Only SEEK_SET is supported" << std::endl;
        return -1;
    }

    // Проверяем, существует ли файл в нашей карте
    if (file_map.find(fd) == file_map.end()) {
        std::cerr << "Error: Invalid file descriptor" << std::endl;
        return -1;
    }

    // Устанавливаем новое смещение в кэше
    file_map[fd].current_offset = offset;

    return offset;  // Возвращаем новое смещение
}

// Синхронизация данных из кэша с диском
int lab2_fsync(int fd) {
    cache_flush(fd);
    return 0;
}
