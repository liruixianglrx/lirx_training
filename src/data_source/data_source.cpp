#include "data_source.h"

#include <dirent.h>

#include <iostream>

#define MARKET_PATH "/workspace/data/Market"
#define ORDERX_PATH "/workspace/data/OrderX"

DataSource::DataSource(std::string stock_code, int date,
                       std::string data_type) {
  m_process_index = 0;

  if (data_type == "Market") {
    m_struct_size = sizeof(XyMarketData);
    m_entry_num = 30;
  }

  // 将股票代码转为可读取的形式，e.g. 10010504 -> 010504.SH
  std::string file_name(stock_code);
  if (file_name[0] == '1') {
    file_name += ".SH";
  } else {
    file_name += ".SZ";
  }
  file_name = file_name.substr(2);

  ReadDataFromFile(file_name, date, data_type);
}

DataSource::~DataSource() { delete[] m_datas[0]; }

void DataSource::ReadDataFromFile(std::string stock_code, int date,
                                  std::string data_type) {
  std::vector<unsigned char> buffer;
  // 从文件中读取相应的数据
  XYData* pData = XYData::New(HOME);
  XYErrorCode error = pData->Read(data_type, stock_code, date, buffer);

  // 将数据重新保存，便于未来进行类型转换
  unsigned char* tbuffer = new unsigned char[buffer.size()];
  for (int k = 0; k < buffer.size(); k++) {
    tbuffer[k] = buffer[k];
  }

  m_size = buffer.size() / m_struct_size;
  if (data_type == "Market") {
    XyMarketData* market_data;
    for (int k = 0; k < m_size; k++) {
      // 获得一个文件中所有data
      m_datas.push_back(tbuffer + k * m_struct_size);
      market_data =
          reinterpret_cast<XyMarketData*>(tbuffer + k * m_struct_size);
      // 获取时刻表
      m_times.push_back(market_data->update_time);
    }
  }
}

// 获得time时刻时最新的数据
unsigned char* DataSource::GetData(int time) {
  if (m_process_index == m_size - 1) {
    return m_datas[m_process_index];
  }
  while (m_process_index + 1 < m_size && m_times[m_process_index + 1] <= time) {
    m_process_index++;
  }

  return m_datas[m_process_index];
}

// time时刻是否有数据更新
bool DataSource::HasNewData(int time) {
  if (m_process_index == m_size - 1) {
    return false;
  }

  if (m_times[m_process_index + 1] <= time) return true;

  return false;
}