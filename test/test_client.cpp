#include "gtest/gtest.h"
#include "MarketDataClient.hpp"
#include <thread>
#include <vector>
#include <set>
#include <mutex>

TEST(MarketDataClientTest, FailsOnNullChannel) {
    auto client = MarketDataClient::createClient(nullptr);
    EXPECT_FALSE(client.ok());
    EXPECT_EQ(client.status().code(), absl::StatusCode::kInternal);
}


TEST(MarketDataClientTest, SuccessOnCreation) {
    auto channel = grpc::CreateChannel("localhost:59999", grpc::InsecureChannelCredentials());
    auto client = MarketDataClient::createClient(channel);
    
    EXPECT_TRUE(client.ok());
}


TEST(MarketDataClientTest, SequentialIds) {
    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());

    auto clientOr1 = MarketDataClient::createClient(channel);
    auto clientOr2 = MarketDataClient::createClient(channel);
    ASSERT_TRUE(clientOr1.ok());
    ASSERT_TRUE(clientOr2.ok());

    auto client1 = std::move(clientOr1.value());
    auto client2 = std::move(clientOr2.value());

    EXPECT_LT(client1.getId(), client2.getId());  // client2 ID > client1 ID
}


TEST(MarketDataClientTest, ConcurrentUniqueIds) {
    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());

    constexpr int numThreads = 4;
    constexpr int clientsPerThread = 100;

    std::vector<int> allIds;
    std::mutex idsMutex;

    auto worker = [&]() {
        for (int i = 0; i < clientsPerThread; ++i) {
            auto clientOr = MarketDataClient::createClient(channel);
            ASSERT_TRUE(clientOr.ok());
            int id = clientOr.value().getId();

            std::lock_guard<std::mutex> lock(idsMutex);
            allIds.push_back(id);
        }
    };

    // Launch threads
    std::vector<std::thread> threads;
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back(worker);
    }
    for (auto &th : threads) {
        th.join();
    }

    // ✅ Verify uniqueness only
    std::set<int> uniqueIds(allIds.begin(), allIds.end());
    EXPECT_EQ(uniqueIds.size(), allIds.size()) << "Duplicate client IDs detected!";
}