#ifndef OS_LAB2_CACHE_H
#define OS_LAB2_CACHE_H

#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstring>
#include <iostream>
#include <sys/stat.h>

#define CACHE_SIZE 64  // Количество страниц в кэше
#define PAGE_SIZE 512 // Размер одной страницы (байт)

struct FileKey {
    dev_t device_id;
    ino_t inode;

    bool operator==(const FileKey &other) const {
        return device_id == other.device_id && inode == other.inode;
    }
};
// Хеш-функция для FileId
namespace std {
    template <>
    struct hash<FileKey> {
        size_t operator()(const FileKey& key) const {
            return hash<dev_t>()(key.device_id) ^ (hash<ino_t>()(key.inode) << 1);
        }
    };
}
// Структура страницы в кэше
struct CachePage {
    int fd;                // Файловый дескриптор
    FileKey key;
    off_t file_offset;      // Смещение в файле
    char data[PAGE_SIZE];   // Данные страницы
    bool is_used;           // Флаг использования (1 - занята, 0 - можно вытеснить)
    bool is_modified;       // Флаг модификации (1 - изменена)
};

// Структура кэша
struct Cache {
    CachePage pages[CACHE_SIZE];
    int clock_hand;
};

extern Cache cache;

FileKey get_file_key(int fd);
void cache_init();
int cache_lookup(int fd, off_t offset);
int cache_find_free_page();
int cache_replace(int fd);
void cache_read(int fd, off_t offset, char *buf, size_t count);
void cache_write(int fd, off_t offset, const char *buf, size_t count);
void cache_flush(int fd);

#endif //OS_LAB2_CACHE_H
