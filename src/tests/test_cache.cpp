#include <gtest/gtest.h>
#include <fstream>
#include "../../include/cache.h"
#include "../../include/tools.h"
#include "../../include/file_worker.h"


class CacheTest : public ::testing::Test {
protected:
    static constexpr const char *test_filename = "test_cache_file.txt";

    void SetUp() override {
        std::ofstream test_file(test_filename);
        ASSERT_TRUE(test_file) << "Failed to create " << test_filename;
        test_file.close();

        cache_init();
    }
    void TearDown() override {
        clear_cache();
        std::remove(test_filename);
    }
};

TEST_F(CacheTest, ReadWriteTest) {
    int fd = lab2_open(CacheTest::test_filename);
    ASSERT_NE(fd, -1) << "Failed to open " << CacheTest::test_filename;


    const char test_data1[PAGE_SIZE] = "First data";
    const char test_data2[PAGE_SIZE] = "Second data";
    char buffer[PAGE_SIZE];

    cache_write(fd, 0, test_data1, PAGE_SIZE);
    cache_write(fd, PAGE_SIZE, test_data2, PAGE_SIZE);

    cache_read(fd, 0, buffer, PAGE_SIZE);
    EXPECT_EQ(memcmp(test_data1, buffer, PAGE_SIZE), 0);

    cache_read(fd, PAGE_SIZE, buffer, PAGE_SIZE);
    EXPECT_EQ(memcmp(test_data2, buffer, PAGE_SIZE), 0);

    EXPECT_EQ(lab2_close(fd), 0);
}

TEST_F(CacheTest, OffsetTest) {
    int fd = lab2_open(CacheTest::test_filename);
    ASSERT_NE(fd, -1) << "Failed to open " << CacheTest::test_filename;

    const char write_data[] = "hhhhh";
    const char exp_data1[] = "hhhaa";
    const char exp_data2[] = "hhBhhaa";
    char read_data[20] = {};

    // Помещаем данные в кэш. Page 0: data - "hhhhh"
    cache_write(fd, 0, write_data, sizeof(write_data) - 1);

    // Помещаем данные в кэш. Page 0: data - "hhhhhaa"
    cache_write(fd, 5, "aa", 2);

    cache_read(fd, 2, read_data, sizeof(write_data) - 1); // Читаем со смещением 2 -> "hhhaa"
    EXPECT_EQ(memcmp(read_data, exp_data1, sizeof (exp_data1)), 0);

    // Помещаем данные в кэш. Page 0: data - "hhBhhaa"
    cache_write(fd, 2, "B", 1);
    cache_read(fd, 0, read_data, 7); // data - "hhhaa"
    EXPECT_EQ(memcmp(read_data, exp_data2, sizeof (exp_data2)), 0);

    EXPECT_EQ(lab2_close(fd), 0);
}

TEST_F(CacheTest, ReplaceTest) {
    int fd = lab2_open(CacheTest::test_filename);
    ASSERT_NE(fd, -1) << "Failed to open " << CacheTest::test_filename;

    const int num_pages = 66;  // Количество страниц превышающее размер кэша (64)
    char test_data[num_pages][PAGE_SIZE];
    char buffer[PAGE_SIZE];

    // Заполняем тестовые данные
    for (int i = 0; i < num_pages; i++) {
        memset(test_data[i], 'A' + (i % 26), 1);
    }

    // Заполняем полностью кэш данными
    for (int i = 0; i < CACHE_SIZE; i++) {
        off_t offset = i * PAGE_SIZE;
        cache_write(fd, offset, test_data[i], PAGE_SIZE);
    }

    // Записываем дополнительные страницы, вызывая замещение в кэше
    for (int i = (num_pages - CACHE_SIZE); i > 0; i--) {
        off_t offset = (num_pages - i) * PAGE_SIZE;
        cache_write(fd, offset, test_data[num_pages - i], PAGE_SIZE);
    }

    // Проверяем корректность данных в замещенных страницах
    for (int i = (num_pages - CACHE_SIZE); i > 0; i--) {
        cache_read(fd, (num_pages - i) * PAGE_SIZE, buffer, PAGE_SIZE);
        EXPECT_EQ(memcmp(test_data[(num_pages - i)], buffer, PAGE_SIZE), 0);
    }
    // Проверяем, что оставшиеся страницы не были заменены
    for (int i = num_pages - CACHE_SIZE; i < num_pages; i++) {
        cache_read(fd, i * PAGE_SIZE, buffer, PAGE_SIZE);
        EXPECT_EQ(memcmp(test_data[i], buffer, PAGE_SIZE), 0);
    }

    EXPECT_EQ(lab2_close(fd), 0);
}
