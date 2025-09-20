#ifndef MARKET_DATA_SERVER_HPP
#define MARKET_DATA_SERVER_HPP

#include "marketdata.grpc.pb.h"

class MarketDataServiceImpl final : public marketdata::MarketData::Service
{
    public:
    grpc::Status Subscribe(grpc::ServerContext *context, 
                            const marketdata::StockRequest *request,
                            grpc::ServerWriter<marketdata::StockPrice> *writer) override;
};

#endif