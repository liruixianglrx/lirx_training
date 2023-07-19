#ifndef LIRX_TRAINING_DATA_SOURCE_DATA_SOURCE
#define LIRX_TRAINING_DATA_SOURCE_DATA_SOURCE

#ifdef _WIN32
#define HOME "Z:"
#define DLL7Z "7z.dll"
#else
#define HOME "/workspace/data"
#define DLL7Z "7z.so"
#endif

#include <dirent.h>

#include <string>
#include <vector>

#include "../OrderBook.h"
#include "../XYData.h"
#include "../XyMarketData.h"

#define MARKET_PATH "/workspace/data/Market"
class DataSource {
 public:
  DataSource(std::string stock_code, int date, std::string data_type);
  ~DataSource();

  bool HasNewData(int time);

  unsigned char* getData(int time);

 private:
  int m_process_index, m_size;
  int m_time;
  int m_struct_size;
  std::vector<unsigned char*> m_datas;
  std::vector<int> m_times;

  void readDataFromFile(std::string stock_code, int date,
                        std::string data_type);
};
#endif  // LIRX_TRAINING_DATA_SOURCE_DATA_SOURCE