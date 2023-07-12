#include "DataSource.h"

#include <dirent.h>

#include <iostream>

#include "../OrderBook.h"
#include "../XYData.h"
#include "../XyMarketData.h"

#define MARKET_PATH "/workspace/data/Market"
#define ORDERX_PATH "/workspace/data/OrderX"

DataSource::DataSource(std::string type, int bsize)
    : data_type(type), buffer_size(bsize) {
  data_buffer.resize(buffer_size);
  start_index = end_index = process_idx = buffer_capacity = 0;

  // find all market files in the folder
  DIR* market_dir;
  std::string market_path;
  if (market_files.empty()) {
    market_path = MARKET_PATH;
    market_dir = opendir(market_path.c_str());
  }

  if (market_dir == nullptr) {
    std::cout << "Failed to open directory." << std::endl;
  }

  // traverse all folders
  dirent* entry;
  while ((entry = readdir(market_dir)) != nullptr) {
    // Filter out items in the current directory and the parent directory
    if (std::string(entry->d_name) == "." ||
        std::string(entry->d_name) == "..") {
      continue;
    }

    // update subdirectories
    market_path = market_path + "/" + entry->d_name;
    date = std::stoi(entry->d_name);

    // load all stock info
    DIR* child_dir = opendir(market_path.c_str());
    dirent* child_entry;
    std::string tmp_str;
    // traverse all folders
    while ((child_entry = readdir(child_dir)) != nullptr) {
      if (std::string(child_entry->d_name) == "." ||
          std::string(child_entry->d_name) == "..") {
        continue;
      }

      // change stock code into proper form
      // e.g 10018003.7z -> 018003.SH
      // when first 2 bits 10 changes into .SH while 20 changes into .SZ
      tmp_str.clear();
      tmp_str = child_entry->d_name;
      tmp_str = tmp_str.substr(2, 6);

      if (child_entry->d_name[0] == '1') {
        tmp_str += ".SH";
      } else {
        tmp_str += ".SZ";
      }
      market_files.emplace_back(tmp_str);
    }
    closedir(child_dir);
  }

  closedir(market_dir);
}

DataSource::~DataSource() {
  for (auto& it : data_buffer) {
    delete[] it;
  }
}
void DataSource::readDataFromFile(const std::string& type,
                                  const std::string& code, int date) {
  std::vector<unsigned char> buffer;
  // read data from proper file
  XYData* pData = XYData::New(HOME);
  XYErrorCode error = pData->Read(type, code, date, buffer);
  // re-store it for future reinterpre_cast
  unsigned char* tbuffer = new unsigned char[buffer.size()];
  for (int i = 0; i < buffer.size(); i++) {
    tbuffer[i] = buffer[i];
  }

  data_buffer[end_index] = tbuffer;
  // update indexs and capacity
  end_index = (end_index + 1) % buffer_size;
  buffer_capacity++;
}

bool DataSource::readNDataFromFile(int n) {
  if (buffer_capacity + n > buffer_size) {
    return false;
  }

  // read N files from
  std::string code;
  for (int i = 0; i < n; i++) {
    if (data_type == "Market") {
      code = market_files[process_idx];
      process_idx++;
      readDataFromFile(data_type, code, date);
    }
  }

  return true;
}

unsigned char* DataSource::getData() {
  unsigned char* tmp = data_buffer[start_index];
  start_index = (start_index + 1) % buffer_size;
  buffer_capacity--;
  return tmp;
}
