#include "../include/cache.h"

Cache cache;
void cache_init() {
    cache.clock_hand = 0;
    for (int i = 0; i < CACHE_SIZE; i++) {
        cache.pages[i].fd = -1;
        cache.pages[i].file_offset = -1;
        cache.pages[i].used = 0;
        cache.pages[i].modified = 0;
    }
}

// Поиск страницы в кэше
int cache_lookup(int fd, off_t offset){
    for (int i = 0; i  < CACHE_SIZE; i++) {
        if (cache.pages[i].fd == fd && cache.pages[i].file_offset == offset) {
            cache.pages[i].used = 1;
            return i;
        }
    }
    return -1;
}

// Поиск свободной страницы в кэше
int cache_find_free_page() {
    for (int i = 0; i < CACHE_SIZE; ++i) {
        if (cache.pages[i].used == 0) {
            return i;  // Свободная страница
        }
    }
    return -1;  // Свободных страниц нет
}

// Замещение страниц по алгоритму CLOCK
int cache_replace() {
    while (true) {
        int i = cache.clock_hand;
        if (cache.pages[i].used == 0) {
            // Если страница изменялась, перед удалением нужно записать изменения на диск
            if (cache.pages[i].modified == 1) {
                _lseek(cache.pages[i].fd, cache.pages[i].file_offset, SEEK_SET);
                _write(cache.pages[i].fd, cache.pages[i].data, BLOCK_SIZE);
            }
            return i;
        }
        cache.pages[i].used = 0;
        cache.clock_hand = (cache.clock_hand + 1) % CACHE_SIZE;
    }
}

// Чтение данных из файла
void cache_read(int fd, off_t offset, char *buf, size_t count) {
    size_t bytes_read = 0;

    while (bytes_read < count) {
        int i = cache_lookup(fd, offset + bytes_read);

        if (i != -1) {
            // Если страница найдена в кэше, копируем данные
            std::memcpy(buf + bytes_read, cache.pages[i].data, BLOCK_SIZE);
            bytes_read += BLOCK_SIZE;
        } else {
            // Если страница не найдена в кэше, нужно ее загрузить
            i = cache_find_free_page();
            if (i == -1) {
                i = cache_replace();  // Используем CLOCK для замены
            }

            cache.pages[i].fd = fd;
            cache.pages[i].file_offset = offset + bytes_read;
            cache.pages[i].used = 1;
            cache.pages[i].modified = 0;

            // Читаем данные с диска в кэш
            _lseek(fd, offset + bytes_read, SEEK_SET);
            _read(cache.pages[i].fd, cache.pages[i].data, BLOCK_SIZE);

            // Копируем данные с кэша в буфер
            std::memcpy(buf + bytes_read, cache.pages[i].data, BLOCK_SIZE);
            bytes_read += BLOCK_SIZE;
        }
    }
}

// Запись данных в кэш
void cache_write(int fd, off_t offset, const char *buf, size_t count) {
    size_t bytes_written = 0;
    while (bytes_written < count) {
        int i = cache_lookup(fd, offset + bytes_written);
        if (i == -1) {
            i = cache_find_free_page();
            if (i == -1) {
                // Вызываем CLOCK если нет свободных страниц
                i = cache_replace();
            }
            cache.pages[i].fd = fd;
            cache.pages[i].file_offset = offset + bytes_written;
        }

        cache.pages[i].used = 1;
        cache.pages[i].modified = 1;

        // Копируем данные с буфера в кэш
        size_t page_offset = (offset + bytes_written) % BLOCK_SIZE;
        size_t to_copy = std::min(count - bytes_written, BLOCK_SIZE - page_offset);
        std::memcpy(cache.pages[i].data + page_offset, buf + bytes_written, to_copy);
//        size_t to_copy = std::min(count - bytes_written, (size_t)BLOCK_SIZE);
//        std::memcpy(cache.pages[i].data, buf + bytes_written, to_copy);

        bytes_written += to_copy;

    }
//    offset += bytes_written;
//    _lseek(fd, *offset, SEEK_SET);
}


// Запись измененных страниц на диск
void cache_flush(int fd) {
    for (int i = 0; i  < CACHE_SIZE; i++) {
        if (cache.pages[i].fd == fd && cache.pages[i].modified == 1) {
            _lseek(fd, cache.pages[i].file_offset, SEEK_SET);
            _write(fd, cache.pages[i].data, BLOCK_SIZE);
            cache.pages[i].modified = 0;
        }
    }
}
