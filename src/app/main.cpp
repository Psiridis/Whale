#include "MarketDataClient.hpp"
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <thread>
#include <vector>

int main(int argc, char** argv) {
    std::string  port;
    if (argc > 1) {
        port = "localhost:" + std::string(argv[1]);
    } else {
        std::cerr << "[App] Unspecified server portal, for example 50051." << std::endl;
    }

    std::cout << "[App] Connecting to " << port << std::endl;

    // Create channel once and share it
    auto channel = grpc::CreateChannel(port, grpc::InsecureChannelCredentials());

    // Stocks we want to subscribe to
    std::vector<std::string> stocks = {"AAPL", "AMZM", "GOOGL", "JNJ", "JPM", "META", "MSFT", "NVDA", "PG", "TSLA"};

    // Keep track of threads
    std::vector<std::thread> threads;

    for (const auto& stock : stocks) {
        threads.emplace_back([&, stock]() {
            auto client_or = MarketDataClient::createClient(channel);
            if (!client_or.ok()) {
                std::cerr << "[App] Failed to create client for " << stock
                          << ": " << client_or.status() << std::endl;
                return;
            }

            MarketDataClient client = *std::move(client_or);

            if (!client.isConnected()) {
                std::cerr << "[App] Client not connected for " << stock << std::endl;
                return;
            }

            std::cout << "[App] Subscribing to " << stock << std::endl;
            client.subscribeToSymbol(stock);
        });
    }

    // Wait for all threads to finish
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    return 0;
}