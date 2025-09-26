#include <gtest/gtest.h>
#include "thread_pool.hpp"

TEST(ThreadPoolTests, TaskReturnsValue) {
    util::thread_pool pool(2);

    auto f = pool.ExecuteTask([] { return 42; });
    EXPECT_EQ(f.get(), 42);
}

TEST(ThreadPoolTests, MultipleTasks) {
    util::thread_pool pool(4);

    std::vector<std::future<int>> futures;
    for (int i = 0; i < 10; i++) {
        futures.push_back(pool.ExecuteTask([i] { return i * i; }));
    }

    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(futures[i].get(), i * i);
    }
}
