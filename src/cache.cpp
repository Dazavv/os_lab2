#include <functional>
#include "../include/cache.h"

Cache cache;

FileKey get_file_key(int fd) {
    struct stat file_stat;
    if (fstat(fd, &file_stat) == -1) {
        std::cout << "fstat failed";
    }
    return {file_stat.st_dev, file_stat.st_ino};
}

void cache_init() {
    cache.clock_hand = 0;
    for (size_t i = 0; i < CACHE_SIZE; i++) {
        cache.pages[i].key = FileKey{};
        cache.pages[i].fd = -1;
        cache.pages[i].file_offset = -1;
        cache.pages[i].is_used = false;
        cache.pages[i].is_modified = false;
    }
}
int cache_find(std::function<bool(const CachePage&)> predicate) {
    for (size_t i = 0; i < CACHE_SIZE; ++i) {
        if (predicate(cache.pages[i])) {
            return i;
        }
    }
    return -1;
}

// Поиск страницы в кэше
int cache_lookup(int fd, off_t offset){
    FileKey key = get_file_key(fd);
    return cache_find([&](const CachePage& page) {
        return page.key.device_id == key.device_id &&
                   page.key.inode == key.inode &&
                   page.file_offset == offset;
    });
}

// Поиск свободной страницы в кэше
int cache_find_free_page() {
    return cache_find([](const CachePage& page) {
        return !page.is_used;
    });
}

// Замещение страниц по алгоритму CLOCK
int cache_replace() {
    for (;;) {
        int i = cache.clock_hand;
        if (!cache.pages[i].is_used) {
            // Если страница изменялась, перед удалением нужно записать изменения на диск
            if (cache.pages[i].is_modified) {
                _lseek(cache.pages[i].fd, cache.pages[i].file_offset, SEEK_SET);
                _write(cache.pages[i].fd, cache.pages[i].data, PAGE_SIZE);
            }
            return i;
        }
        cache.pages[i].is_used = false;
        cache.clock_hand = (cache.clock_hand + 1) % CACHE_SIZE;
    }
}

// Чтение данных из файла
void cache_read(int fd, off_t offset, char *buf, size_t count) {
    size_t bytes_read = 0;
    FileKey key = get_file_key(fd);

    while (bytes_read < count) {
        off_t page_start = (offset + bytes_read) / PAGE_SIZE * PAGE_SIZE;
        int i = cache_lookup(fd, page_start + bytes_read);

        if (i != -1) {
            size_t page_offset = (offset + bytes_read) % PAGE_SIZE;
            size_t to_copy = std::min(count - bytes_read, PAGE_SIZE - page_offset);
            // Если страница найдена в кэше, копируем данные
            std::memcpy(buf + bytes_read, cache.pages[i].data + page_offset, to_copy);
            bytes_read += to_copy;
        } else {
            // Если страница не найдена в кэше, нужно ее загрузить
            i = cache_find_free_page();
            if (i == -1) i = cache_replace();  // Используем CLOCK для замены

            cache.pages[i].key = key;
            cache.pages[i].fd = fd;
            cache.pages[i].file_offset = page_start;
            cache.pages[i].is_used = true;
            cache.pages[i].is_modified = false;

            // Читаем данные с диска в кэш
            _lseek(fd, page_start, SEEK_SET);
            _read(fd, cache.pages[i].data, PAGE_SIZE);

            // Копируем данные с кэша в буфер
            std::memcpy(buf + bytes_read, cache.pages[i].data, PAGE_SIZE);
            bytes_read += PAGE_SIZE;
        }
    }
}

// Запись данных в кэш
void cache_write(int fd, off_t offset, const char *buf, size_t count) {
    size_t bytes_written = 0;
    FileKey key = get_file_key(fd);

    while (bytes_written < count) {
        off_t offset1 = (offset + bytes_written) / PAGE_SIZE * PAGE_SIZE;
        int i = cache_lookup(fd, offset1 + bytes_written);
        if (i == -1) {
            i = cache_find_free_page();
            if (i == -1) {
                // Вызываем CLOCK если нет свободных страниц
                i = cache_replace();
            }

            cache.pages[i].fd = fd;
            cache.pages[i].key = key;
            cache.pages[i].file_offset = offset1;
            cache.pages[i].is_used = true;
            cache.pages[i].is_modified = false;

            _lseek(fd, cache.pages[i].file_offset, SEEK_SET);
            _read(fd, cache.pages[i].data, PAGE_SIZE);
        }

        cache.pages[i].is_used = true;
        cache.pages[i].is_modified = true;

        // Копируем данные с буфера в кэш
        size_t page_offset = (offset + bytes_written) % PAGE_SIZE;
        size_t to_copy = std::min(count - bytes_written, PAGE_SIZE - page_offset);

        std::memcpy(cache.pages[i].data + page_offset, buf + bytes_written, to_copy);
        bytes_written += to_copy;

    }
}


// Запись измененных страниц на диск
void cache_flush(int fd) {
    FileKey key = get_file_key(fd);
    for (size_t i = 0; i < CACHE_SIZE; i++) {
        if (cache.pages[i].key == key && cache.pages[i].is_modified) {
            _lseek(fd, cache.pages[i].file_offset, SEEK_SET);
            _write(fd, cache.pages[i].data, PAGE_SIZE);
            cache.pages[i].is_modified = false;
        }
    }
}
