#include <unordered_map>

#include "../include/file_worker.h"
#include "../include/cache.h"

struct FileInfo {
    off_t current_offset;  // Смещение для данного файла
};
// Хранилище данных о файлах (сопоставляем fd с FileInfo)
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
    // получить файловый дескриптор
    int fd = ::_open_osfhandle((intptr_t) file, _O_RDWR);
    return fd;
}

// Закрытие файла по хэндлу
int lab2_close(int fd) {
    cache_flush(fd);

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
    off_t offset = _lseek(fd, 0, SEEK_CUR);
    cache_read(fd, offset, (char *) buf, count);
    _lseek(fd, offset + count, SEEK_SET);
    return count;
}

// Запись данных в файл
ssize_t lab2_write(int fd, const void *buf, size_t count) {
    off_t offset = _lseek(fd, 0, SEEK_CUR);  // Получаем текущий offset
    if (offset == -1) return -1;
    cache_write(fd, offset, (const char *) buf, count);
    _lseek(fd, count, SEEK_CUR);
    return count;
}

// Перестановка позиции указателя на данные файла
off_t lab2_lseek(int fd, off_t offset, int whence) {
    off_t new_offset = _lseek(fd, offset, whence);
    if (new_offset == -1) {
        std::cerr << "Error: lseek failed for fd " << fd << " with offset " << offset << std::endl;
    }

    return new_offset;
//    if (whence == SEEK_SET) {
//        // Устанавливаем позицию указателя на абсолютное смещение
//        file_map[fd].current_offset = offset;
//    } else {
//        return -1;  // Не поддерживается других флагов
//    }
//    return file_map[fd].current_offset; // Возвращаем новое смещение
}

// Синхронизация данных из кэша с диском
int lab2_fsync(int fd) {
    cache_flush(fd);
    return 0;
}
