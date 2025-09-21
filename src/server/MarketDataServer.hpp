#ifndef MARKET_DATA_SERVER_HPP
#define MARKET_DATA_SERVER_HPP

#include "marketdata.grpc.pb.h"
#include <string>

class StockData
{
    public:
    private:
        std::string m_date;
        double m_adj_close;
        double m_close;
        double m_high;
        double m_low;
        double m_open;
        long   m_volume;
};

class MarketDataServiceImpl final : public marketdata::MarketData::Service
{
    public:
    grpc::Status Subscribe(grpc::ServerContext *context, 
                            const marketdata::StockRequest *request,
                            grpc::ServerWriter<marketdata::StockPrice> *writer) override;

    private:
        std::unordered_map<std::string, StockData> m_stock_data;
};

#endif