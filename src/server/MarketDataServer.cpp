#include "MarketDataServer.hpp"
#include <grpcpp/grpcpp.h>
#include <chrono>
#include <mutex>
#include <thread>

static std::mutex cout_mutex;

grpc::Status MarketDataServiceImpl::Subscribe(
    grpc::ServerContext *context, const marketdata::StockRequest *request,
    grpc::ServerWriter<marketdata::StockPrice> *writer)
{
  std::cout << "[Server] Client subscribed to: " << request->symbol() << "\n";

  // Random number generator for simulating stock prices
  std::mt19937 rng(std::random_device{}());
  std::uniform_real_distribution<double> dist(100.0, 200.0);

  while (!context->IsCancelled()) {
    marketdata::StockPrice price;
    auto time =
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
            .count();

    price.set_symbol(request->symbol());
    price.set_price(dist(rng));
    price.set_timestamp_ns(time);
        
    std::uniform_real_distribution<double> time_ms(100., 1000.);
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(time_ms(rng))));
    
    writer->Write(price);
    {
      std::lock_guard<std::mutex> lock(cout_mutex);
      std::cout << "[Server] Sent update for " << request->symbol()
                << " price: " << price.price() << std::endl;
    }
  }

  std::cout << "[Server] Subscription ended for: " << request->symbol()
            << std::endl;

  return grpc::Status::OK;
}