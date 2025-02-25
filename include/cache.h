#ifndef OS_LAB2_CACHE_H
#define OS_LAB2_CACHE_H

#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstring>
#include <iostream>

#define CACHE_SIZE 64  // Количество страниц в кэше
#define BLOCK_SIZE 512 // Размер одной страницы

// Структура страницы в кэше
typedef struct {
    int fd;                // Файловый дескриптор
    off_t file_offset;     // Смещение в файле
    char data[BLOCK_SIZE]; // Данные страницы
    int used;              // Флаг использования (1 - занята, 0 - можно вытеснить)
    int modified;          // Флаг модификации (1 - изменена)
} CachePage;

// Структура кэша
typedef struct {
    CachePage pages[CACHE_SIZE];
    int clock_hand;        // "Стрелка" алгоритма CLOCK
} Cache;

extern Cache cache;

void cache_init();
int cache_lookup(int fd, off_t offset);
int cache_find_free_page();
int cache_replace();
void cache_read(int fd, off_t offset, char *buf, size_t count);
void cache_write(int fd, off_t offset, const char *buf, size_t count);
void cache_flush(int fd);

#endif //OS_LAB2_CACHE_H
