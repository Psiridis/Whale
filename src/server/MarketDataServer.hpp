#ifndef MARKET_DATA_SERVER_HPP
#define MARKET_DATA_SERVER_HPP

#include "marketdata.grpc.pb.h"
#include <string>
#include <vector>

class StockData {
 public:
  StockData(const std::string &date, double adjusted_close, double close,
            double high, double low, double open, double volume)
      : m_date(date),
        m_adj_close(adjusted_close),
        m_close(close),
        m_high(high),
        m_low(low),
        m_open(open),
        m_volume(volume) {}

  const std::string &date() const { return m_date; }
  double open() const { return m_open; }
  double high() const { return m_high; }
  double low() const { return m_low; }
  double close() const { return m_close; }
  double adj_close() const { return m_adj_close; }
  long long volume() const { return m_volume; }

 private:
  std::string m_date;
  double m_adj_close;
  double m_close;
  double m_high;
  double m_low;
  double m_open;
  long long m_volume;
};

class MarketDataServiceImpl final : public marketdata::MarketData::Service
{
    public:
    grpc::Status Subscribe(grpc::ServerContext *context, 
                            const marketdata::StockRequest *request,
                            grpc::ServerWriter<marketdata::StockPrice> *writer) override;

    void load_data(const std::string &file);
    private:
        std::unordered_map<std::string, std::vector<StockData>> m_stock_data;
};

#endif