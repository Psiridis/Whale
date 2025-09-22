#include "MarketDataServer.hpp"
#include <grpcpp/grpcpp.h>
#include <chrono>
#include <mutex>
#include <thread>
#include <fstream>
#include <sstream>

static std::mutex cout_mutex;

void MarketDataServiceImpl::load_data(const std::string &filepath)
{
    std::ifstream file(filepath);

    if (!file.is_open()) {
        std::cout << "Failed to open file: " << filepath << "\n";
        return;
    }

    std::string line;

    // First line: symbol
    std::string symbol;
    if (!std::getline(file, symbol)) {
        std::cerr << "File empty: " << filepath << "\n";
        return;
    }

    // Second line: header -> skip
    std::getline(file, line);

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string date;
        std::string tmp;
        
        //DATE
        std::getline(ss, date, ',');
        //ADJ CLOSE
        std::getline(ss, tmp, ',');
        double adj_close = std::stod(tmp);
        //CLOSE        
        std::getline(ss, tmp, ',');
        double close = std::stod(tmp);
        //HIGH
        std::getline(ss, tmp, ',');
        double high = std::stod(tmp);
        //LOW
        std::getline(ss, tmp, ',');
        double low = std::stod(tmp);
        //OPEN
        std::getline(ss, tmp, ',');
        double open = std::stod(tmp);
        //VOLUME
        std::getline(ss, tmp, ',');
        long volume = std::stol(tmp);

        m_stock_data[symbol].emplace_back(StockData(date, adj_close, close, high, low, open, volume));
    }

    /*
    for (const auto &[symbol, stock_data] : m_stock_data) {
      std::cout << "Symbol: " << symbol << std::endl;
      for (auto const &data : stock_data) {
        std::cout <<
        "Date: "      << data.date()      << ", " <<
        "Adj Close: " << data.adj_close() << ", " <<
        "Close: "     << data.close()     << ", " <<
        "High: "      << data.high()      << ", " <<
        "Low: "       << data.low()       << ", " <<
        "Open: "      << data.open()      << ", " <<
        "Volume: "    << data.volume()    << std::endl;
      }
    }
    */
}

const std::unordered_map<std::string, std::vector<StockData>> &
MarketDataServiceImpl::getStockData() const {
  return  m_stock_data;
}

const std::vector<StockData> &MarketDataServiceImpl::getStockData(
    const std::string &symbol) const {
  std::vector<StockData> empty;
  auto it = m_stock_data.find(symbol);
  return (it != m_stock_data.end()) ? it->second : empty;
}

grpc::Status MarketDataServiceImpl::Subscribe(
    grpc::ServerContext *context, const marketdata::StockRequest *request,
    grpc::ServerWriter<marketdata::StockPrice> *writer)
{
  std::cout << "[Server] Client subscribed to: " << request->symbol() << "\n";

  // Random number generator for generating delays
  std::mt19937 rng(std::random_device{}());

  const std::vector<StockData>& stocks = getStockData(request->symbol());

  if (stocks.empty()) {
    return grpc::Status(grpc::StatusCode::NOT_FOUND, "Symbol not found");
  }

  for (const auto &stock_data : stocks) {
    if (context->IsCancelled()) break;

    marketdata::StockPrice price;
    auto now_ns =
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
            .count();

    price.set_symbol(request->symbol());
    price.set_adjustedclose(stock_data.adj_close());
    price.set_close(stock_data.close());
    price.set_high(stock_data.high());
    price.set_low(stock_data.low());
    price.set_open(stock_data.open());
    price.set_volume(stock_data.volume());
    price.set_timestamp_ns(now_ns);

    std::uniform_real_distribution<double> time_ms(100., 1000.);
    std::this_thread::sleep_for(
        std::chrono::milliseconds(static_cast<int>(time_ms(rng))));

    writer->Write(price);
    {
      std::lock_guard<std::mutex> lock(cout_mutex);
      std::cout << "[Server] Sent update for " << request->symbol() << ", " <<
        "Date: "      << stock_data.date()      << ", " <<
        "Adj Close: " << price.adjustedclose()  << ", " <<
        "Close: "     << price.close()     << ", " <<
        "High: "      << price.high()      << ", " <<
        "Low: "       << price.low()       << ", " <<
        "Open: "      << price.open()      << ", " <<
        "Volume: "    << price.volume()    << std::endl;
    }
  }

  std::cout << "[Server] Subscription ended for: " << request->symbol()
            << std::endl;

  return grpc::Status::OK;
}