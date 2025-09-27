#include "MarketDataClient.hpp"
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <thread>
#include <vector>
#include "utilities/thread_pool.hpp"

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
    std::vector<std::string> stocks = {
        "AAPL",
        "MSFT", 
        "GOOGL", 
        "AMZN", 
        "META", 
        "JPM", 
        "JNJ", 
        "NVDA", 
        "PG", 
        "TSLA"
    };

    util::thread_pool thread_pool(stocks.size());

     std::vector<std::future<void>> futures;

    for (const auto& stock : stocks) {
        auto fut = thread_pool.ExecuteTask([&, stock]() {
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

        futures.push_back(std::move(fut));
    }

    // Wait for all tasks to finish
    for (auto& f : futures) {
        f.get();  // blocks until that task completes
    }

    return 0;
}