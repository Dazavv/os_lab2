#include <gtest/gtest.h>
#include "../../include/cache.h"

class CacheTest : public ::testing::Test {
protected:
    void SetUp() override {
        cache_init();
    }
};

TEST_F(CacheTest, ReadWriteTest) {
    const char test_data1[BLOCK_SIZE] = "First data";
    const char test_data2[BLOCK_SIZE] = "Second data";
    char buffer[BLOCK_SIZE];

    cache_write(0, 0, test_data1, BLOCK_SIZE);
    cache_write(0, BLOCK_SIZE, test_data2, BLOCK_SIZE);

    cache_read(0, 0, buffer, BLOCK_SIZE);
    EXPECT_EQ(memcmp(test_data1, buffer, BLOCK_SIZE), 0);

    cache_read(0, BLOCK_SIZE, buffer, BLOCK_SIZE);
    EXPECT_EQ(memcmp(test_data2, buffer, BLOCK_SIZE), 0);
}

TEST_F(CacheTest, ReplaceTest) {
    const int num_pages = 66;  // Количество страниц превышающее размер кэша (64)
    char test_data[num_pages][BLOCK_SIZE];
    char buffer[BLOCK_SIZE];

    // Заполняем тестовые данные
    for (int i = 0; i < num_pages; i++) {
        memset(test_data[i], 'A' + (i % 26), 1);
    }

    // Заполняем полностью кэш данными
    for (int i = 0; i < CACHE_SIZE; i++) {
        off_t offset = i * BLOCK_SIZE;
        cache_write(0, offset, test_data[i], BLOCK_SIZE);
    }

    // Записываем дополнительные страницы, вызывая замещение в кэше
    for (int i = (num_pages - CACHE_SIZE); i > 0; i--) {
        off_t offset = (num_pages - i) * BLOCK_SIZE;
        cache_write(0, offset, test_data[num_pages - i], BLOCK_SIZE);
    }

    // Проверяем корректность данных в замещенных страницах
    for (int i = (num_pages - CACHE_SIZE); i > 0; i--) {
        cache_read(0, (num_pages - i) * BLOCK_SIZE, buffer, BLOCK_SIZE);
        EXPECT_EQ(memcmp(test_data[(num_pages - i)], buffer, BLOCK_SIZE), 0);
    }
    // Проверяем, что оставшиеся страницы не были заменены
    for (int i = num_pages - CACHE_SIZE; i < num_pages; i++) {
        cache_read(0, i * BLOCK_SIZE, buffer, BLOCK_SIZE);
        EXPECT_EQ(memcmp(test_data[i], buffer, BLOCK_SIZE), 0);
    }
}
