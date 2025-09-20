#include "gtest/gtest.h"
#include "MarketDataClient.hpp"

TEST(MarketDataClientTest, FailsOnNullChannel) {
    auto client = MarketDataClient::createClient(nullptr);
    EXPECT_FALSE(client.ok());
    EXPECT_EQ(client.status().code(), absl::StatusCode::kInternal);
}

TEST(MarketDataClientTest, FailsOnUnreachableServer) {
    // Try connecting to a port where no server is running
    auto channel = grpc::CreateChannel("localhost:59999", grpc::InsecureChannelCredentials());
    auto client = MarketDataClient::createClient(channel);

    EXPECT_FALSE(client.ok());
    EXPECT_EQ(client.status().code(), absl::StatusCode::kUnavailable);
}

TEST(MarketDataClientTest, SucceedsWhenServerRunning) {
    // This requires your server to be running at localhost:50051
    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    auto client = MarketDataClient::createClient(channel);

    if (client.ok()) {
        EXPECT_TRUE(client->isConnected());
    } else {
        // If no server is up, this will fail with kUnavailable
        EXPECT_EQ(client.status().code(), absl::StatusCode::kUnavailable);
    }
}