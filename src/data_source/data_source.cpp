#include "data_source.h"

#include <dirent.h>

#include <iostream>

#define MARKET_PATH "/workspace/data/Market"
#define ORDERX_PATH "/workspace/data/OrderX"

DataSource::DataSource(std::string stock_code, int date,
                       std::string data_type) {
  m_process_index = 0;
  m_time = 80000000;

  if (data_type == "Market") {
    m_struct_size = sizeof(XyMarketData);
  }
  std::string file_name(stock_code);
  if (file_name[0] == '1') {
    file_name += ".SH";
  } else {
    file_name += ".SZ";
  }
  file_name = file_name.substr(2);

  readDataFromFile(file_name, date, data_type);
}

DataSource::~DataSource() { delete[] m_datas[0]; }

void DataSource::readDataFromFile(std::string stock_code, int date,
                                  std::string data_type) {
  std::vector<unsigned char> buffer;
  // read data from proper file
  XYData* pData = XYData::New(HOME);
  XYErrorCode error = pData->Read(data_type, stock_code, date, buffer);
  // re-store it for future reinterpre_cast
  unsigned char* tbuffer = new unsigned char[buffer.size()];
  for (int i = 0; i < buffer.size(); i++) {
    tbuffer[i] = buffer[i];
  }

  m_size = buffer.size() / m_struct_size;
  if (data_type == "Market") {
    XyMarketData* market_data;
    for (int i = 0; i < m_size; i++) {
      m_datas.push_back(tbuffer + i * m_struct_size);
      market_data =
          reinterpret_cast<XyMarketData*>(tbuffer + i * m_struct_size);
      m_times.push_back(market_data->update_time);
    }
  }
}

unsigned char* DataSource::getData(int time) {
  if (m_process_index == m_size - 1) {
    return m_datas[m_process_index];
  }
  while (m_process_index + 1 < m_size && m_times[m_process_index + 1] <= time) {
    m_process_index++;
  }

  return m_datas[m_process_index];
  // for (int i = 0; i < m_size - 1; i++)
  //   if (m_times[i] <= time && m_times[i + 1] > time) {
  //     return m_dates[i];
  //   }

  // return m_dates[m_size - 1];
}

bool DataSource::HasNewData(int time) {
  if (m_process_index == m_size - 1) {
    return false;
  }

  if (m_times[m_process_index + 1] <= time) return true;

  return false;
}