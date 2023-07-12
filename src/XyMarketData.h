#pragma pack(1)

/**
 * 共享内存元数据
 * 共享内存中为二维表或三维表数据
 */
struct XyMarketData {
  XyMarketData() {}
  char wind_code[32];               // 600001.SH
  char code[32];                    // 原始Code
  int32_t action_day;               // 业务发生日(自然日)
  int32_t trading_day;              // 交易日
  int32_t update_time;              // 时间(HHMMSSmmm)
  int32_t status;                   // 状态
  uint32_t pre_close;               // 前收盘价
  uint32_t open;                    // 开盘价
  uint32_t high;                    // 最高价
  uint32_t low;                     // 最低价
  uint32_t last_price;              // 最新价
  uint32_t ask_price[10];           // 申卖价
  uint32_t ask_vol[10];             // 申卖量
  uint32_t bid_price[10];           // 申买价
  uint32_t bid_volume[10];          // 申买量
  uint32_t num_trades;              // 成交笔数
  int64_t volume;                   // 成交总量
  int64_t turnover;                 // 成交总金额
  int64_t total_bid_vol;            // 委托买入总量
  int64_t total_ask_vol;            // 委托卖出总量
  uint32_t weighted_avg_bid_price;  // 加权平均委买价格
  uint32_t weighted_avg_ask_price;  // 加权平均委卖价格
  int32_t iopv;                     // IOPV净值估值
  int32_t yield_to_maturity;        // 到期收益率
  uint32_t high_limited;            // 涨停价
  uint32_t low_limited;             // 跌停价
  char prefix[4];                   // 证券信息前缀
  int32_t syl1;                     // 市盈率1
  int32_t syl2;                     // 市盈率2
  int32_t sd2;                      // 升跌2（对比上一笔）
};
#pragma pack()