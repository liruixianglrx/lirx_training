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

#define MARKET_PATH "/workspace/data/Market"
class DataSource {
 public:
  DataSource(std::string type, int bsize);
  ~DataSource();
  void readDataFromFile(const std::string& type, const std::string& code,
                        int date);

  // read N data files from designated folder
  bool readNDataFromFile(int n);

  unsigned char* getData();

  int notSentSize();

 private:
  // TODO : static ?
  std::vector<std::string> market_files;
  // static std::vector<string> orderx_files;

  // store market data from read file
  // circular queue
  std::vector<unsigned char*> data_buffer;

  // Which type of data,e.g. market,orderx,transx
  // TODO:template function for different data_type?
  std::string data_type;

  int start_index, end_index, process_idx;
  int buffer_size, buffer_capacity;
  int date;
};
#endif  // LIRX_TRAINING_DATA_SOURCE_DATA_SOURCE