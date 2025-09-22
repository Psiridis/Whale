#include "gtest/gtest.h"
#include "MarketDataServer.hpp"



TEST(MarketDataServerTest, Dummy) {
    MarketDataServiceImpl service;

    std::string filepath = std::string(TESTING_CMAKE_CURRENT_SOURCE_DIR) + "/sample.csv";
    service.load_data(filepath);

    const auto& data = service.getStockData("AAPL");
    ASSERT_EQ(data.size(), 2);

    EXPECT_EQ(data[0].date(), "2020-09-21");
    EXPECT_DOUBLE_EQ(data[0].adj_close(), 107.0767822265625);
    EXPECT_DOUBLE_EQ(data[0].close(), 110.08000183105469);
    EXPECT_DOUBLE_EQ(data[0].high(), 110.19000244140625);
    EXPECT_DOUBLE_EQ(data[0].low(), 103.0999984741211);
    EXPECT_DOUBLE_EQ(data[0].open(), 104.54000091552734);
    EXPECT_EQ(data[0].volume(), 195713800);

    EXPECT_EQ(data[1].date(), "2020-09-22");
    EXPECT_DOUBLE_EQ(data[1].adj_close(), 108.75956726074219);
    EXPECT_DOUBLE_EQ(data[1].close(), 111.80999755859375);
    EXPECT_DOUBLE_EQ(data[1].high(), 112.86000061035156);
    EXPECT_DOUBLE_EQ(data[1].low(), 109.16000366210938);
    EXPECT_DOUBLE_EQ(data[1].open(), 112.68000030517578);
    EXPECT_EQ(data[1].volume(), 183055400);
}
