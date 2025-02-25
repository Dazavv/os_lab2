#include "../include/tools.h"
#include "../include/cache.h"

// Функция для отладки и вывода текущего состояния кэша
void cache_debug_print(int count) {
    std::cout << "Cache state:" << std::endl;
    for (int i = 0; i < count; ++i) {
        std::cout << "Page " << i << ": " << "flags used - " << cache.pages[i].used << ", modified - " << cache.pages[i].modified << ", fd - " << cache.pages[i].fd << ", offset - " << cache.pages[i].file_offset <<", data: \"" << cache.pages[i].data << "\"" << std::endl;
    }
}