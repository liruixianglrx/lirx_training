#ifndef LIRX_TRAINING_DATA_SOURCE_DATA_SOURCE_H
#define LIRX_TRAINING_DATA_SOURCE_DATA_SOURCE_H

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

  // time时刻是否有数据更新
  bool HasNewData(int time);

  // 获得time时刻时最新的数据
  unsigned char* GetData(int time);

  // 按照数据的index获得数据
  unsigned char* GetDataByIndex(int index) { return m_datas[index]; }
  int m_entry_num;

 private:
  // m_process_index,当前时刻处理的数据序号
  // m_size，总共有多少个数据
  int m_process_index, m_size;

  // 数据类型大小
  int m_struct_size;

  // 保存文件中的所有数据
  std::vector<unsigned char*> m_datas;

  // 文件中数据的时刻表
  std::vector<int> m_times;

  // 从文件中读取数据，指定股票代码，日期，数据类型
  void ReadDataFromFile(std::string stock_code, int date,
                        std::string data_type);
};
#endif  // LIRX_TRAINING_DATA_SOURCE_DATA_SOURCE_H