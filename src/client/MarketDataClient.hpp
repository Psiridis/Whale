#ifndef MARKET_DATA_CLIENT_HPP
#define MARKET_DATA_CLIENT_HPP

#include <memory>
#include "grpcpp/grpcpp.h"
#include "marketdata.grpc.pb.h"
#include "absl/status/statusor.h"
#include <atomic>

class MarketDataClient
{
    public:
        static absl::StatusOr<MarketDataClient> createClient(std::shared_ptr<grpc::Channel> channel);
        
        [[nodiscard]] bool isConnected() const;

        void subscribeToSymbol(const std::string& symbol);

    private:
        explicit MarketDataClient(std::shared_ptr<grpc::Channel> channel, u_int64 id);
    private:
        std::unique_ptr<marketdata::MarketData::Stub> m_stub;
        uint64_t m_id;

        // Static atomic counter to generate unique IDs
        static std::atomic<uint64_t> s_nextId;
};


#endif