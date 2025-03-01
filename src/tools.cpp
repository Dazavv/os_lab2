#include "../include/tools.h"
#include "../include/cache.h"
#include "../include/file_worker.h"

// Функция для отладки и вывода текущего состояния кэша
void cache_debug_print(int count) {
    std::cout << "Cache state:" << std::endl;
    for (int i = 0; i < count; ++i) {
        std::cout << "Page " << i << ": " << "flags used - " << cache.pages[i].is_used << ", modified - " << cache.pages[i].is_modified << ", fd - " << cache.pages[i].fd << ", offset - " << cache.pages[i].file_offset <<", data: \"" << cache.pages[i].data << "\"" << std::endl;
    }
}

void clear_cache() {
    cache.clock_hand = 0;
    for (size_t i = 0; i < CACHE_SIZE; i++) {
        cache.pages[i].key = FileKey{};
        cache.pages[i].fd = -1;
        cache.pages[i].file_offset = -1;
        cache.pages[i].is_used = false;
        cache.pages[i].is_modified = false;
        memset(cache.pages[i].data, 0, PAGE_SIZE);
    }
}
