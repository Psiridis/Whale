#include "MarketDataClient.hpp"
#include <mutex>

// Initialize static counter
std::atomic<uint64_t> MarketDataClient::s_nextId{1};
static std::mutex cout_mutex;

MarketDataClient::MarketDataClient(std::shared_ptr<grpc::Channel> channel, uint64_t id)
: m_stub(marketdata::MarketData::NewStub(channel)),
  m_id(id)
{
}

absl::StatusOr<MarketDataClient> MarketDataClient::createClient(std::shared_ptr<grpc::Channel> channel)
{
    if (!channel) {
        return absl::InternalError("Invalid channel.");
    }

    return MarketDataClient(channel, s_nextId.fetch_add(1));
}

bool MarketDataClient::isConnected() const
{
    return (nullptr != m_stub); 
}

void MarketDataClient::subscribeToSymbol(const std::string& symbol) {
    grpc::ClientContext context;
    marketdata::StockRequest request;
    request.set_symbol(symbol);
    
    
    std::unique_ptr<grpc::ClientReader<marketdata::StockPrice>> reader(
        m_stub->Subscribe(&context, request));
        
    marketdata::StockPrice price;

    while (reader->Read(&price)) {
      {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::ostringstream oss;

        oss << "[Client#" << m_id << "][" << symbol
            << "] Received price: " << price.price() << " @ "
            << price.timestamp_ns();

        std::cout << oss.str() << std::endl;
      }
    }

    grpc::Status status = reader->Finish();
    {
      std::lock_guard<std::mutex> lock(cout_mutex);
      if (!status.ok()) {
        std::cerr << "[Client#" << m_id << "][" << symbol
                  << "] Subscription failed: " << status.error_message()
                  << std::endl;
      } else {
        std::cout << "[Client#" << m_id << "][" << symbol
                  << "] Subscription ended" << std::endl;
      }
    }
}
