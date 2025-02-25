#include <gtest/gtest.h>
#include "../../include/file_worker.h"
#include "../../include/cache.h"
#include "../../include/tools.h"
#include <fstream>
#include <cstdio>

class TestEnvironment : public ::testing::Environment {
public:
    static constexpr const char *test_filename = "test_file.txt";

    void SetUp() override {
        std::ofstream test_file(test_filename);
        ASSERT_TRUE(test_file) << "Failed to create test file";
        test_file.close();

        cache_init();
    }

    void TearDown() override {
        std::remove(test_filename);
    }
};

// Регистрируем глобальное окружение
::testing::Environment* const test_env = ::testing::AddGlobalTestEnvironment(new TestEnvironment());

TEST(FileWorkerTest, Lab2OpenCloseTest) {
    int fd = lab2_open(TestEnvironment::test_filename);
    ASSERT_NE(fd, -1) << "Failed to open test file";
    EXPECT_EQ(lab2_close(fd), 0);
}

TEST(FileWorkerTest, Lab2WriteTest) {
    int fd = lab2_open(TestEnvironment::test_filename);
    ASSERT_NE(fd, -1);

    const char test_data[] = "good day, good life";
    ssize_t bytes_written = lab2_write(fd, test_data, sizeof(test_data) - 1);
    EXPECT_EQ(bytes_written, sizeof(test_data) - 1);

    char buffer[BLOCK_SIZE] = {};
    lab2_read(fd, buffer, sizeof(test_data) - 1);
    EXPECT_EQ(memcmp(test_data, buffer, sizeof(test_data) - 1), 0);

    EXPECT_EQ(lab2_close(fd), 0);
}

//TEST(FileWorkerTest, Lab2SeekTest) {
////    int fd = lab2_open(TestEnvironment::test_filename);
//    int fd = lab2_open("D:\\dasha\\Projects_CLion\\os_lab2\\cmake-build-debug\\test_file.txt");
//    ASSERT_NE(fd, -1);
//
//    const char test_data[] = "Data after seek";
//    lab2_write(fd, test_data, sizeof(test_data) - 1);
//    lab2_fsync(fd);
////    off_t current_pos = _lseek(fd, 0, SEEK_CUR);
////    std::cout << "Current position after write: " << current_pos << std::endl;
////    off_t size = _lseek(fd, 0, SEEK_END);
////    std::cout << "End position of file: " << size << std::endl;
//    if (_lseek(fd, 0, SEEK_SET) == -1) {
//        perror("lseek failed - ");
//    }
////    std::cout <<"FD - " << fd << "\nLseek in file = " << _lseek(fd, 1, SEEK_SET) << std::endl;
//
//
//    EXPECT_EQ(lab2_lseek(fd, 0, SEEK_SET), 0);
//    EXPECT_EQ(lab2_lseek(fd, 1, SEEK_SET), 1);
//    EXPECT_EQ(lab2_lseek(fd, 3, SEEK_CUR), 8);
//    EXPECT_EQ(lab2_lseek(fd, -3, SEEK_END), sizeof(test_data) - 3);
//
//    EXPECT_EQ(lab2_close(fd), 0);
//}

TEST(FileWorkerTest, Lab2FsyncTest) {
    int fd = lab2_open(TestEnvironment::test_filename);
    ASSERT_NE(fd, -1);

    const char test_data[] = "hell";
    lab2_write(fd, test_data, sizeof(test_data) - 1);
    EXPECT_EQ(lab2_fsync(fd), 0);

    EXPECT_EQ(lab2_close(fd), 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
