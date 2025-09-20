#include <gtest/gtest.h>
#include <chrono>

TEST(LatencyTest, NonNegativeLatency) {
    auto now = std::chrono::high_resolution_clock::now();
    auto past = now - std::chrono::microseconds(10);

    auto latency = std::chrono::duration_cast<std::chrono::nanoseconds>(now - past).count();
    EXPECT_GE(latency, 0);
}
