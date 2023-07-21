#include "data_source.h"

// g++ test.cpp data_source.cpp -g -o test -L../lib -lXYData
int main() {
  DataSource ds("10010504", 20230704, "Market");
  XyMarketData* MarketData =
      reinterpret_cast<XyMarketData*>(ds.GetData(80000001));
}