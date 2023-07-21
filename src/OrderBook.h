#pragma once
#include <cassert>
#include <climits>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "TDF25/TDFAPIStruct.h"
#include "XYData.h"

#define MKT_START 93000000
#define MKT_FINAL 145700000
#define MKT_CLOSE 150000000

#define STATUS_CALL 0  // 集合竞价交易
#define STATUS_CONT 1  // 连续竞价交易

#define NEW_RULES_DAY 20230410  // 全面注册制新规首日

// XYAsset Extensions
#pragma pack(push)
#pragma pack(1)
struct TDF_TRANSACTION_X : public TDF_TRANSACTION {
  int32_t nChannel;   // channel id
  int64_t nBizIndex;  // 业务编号
};

struct TDF_ORDER_X : public TDF_ORDER {
  int32_t nChannel;     // channel id
  int64_t nOrderOriNo;  // 原始订单号
  int64_t nBizIndex;    // 业务编号
};
#pragma pack(pop)

inline bool endswith(const std::string& s1, const std::string& s2) {
  return s1.size() >= s2.size() && s1.substr(s1.size() - s2.size()) == s2;
}

inline bool startswith(const std::string& s1, const std::string& s2) {
  return s1.size() >= s2.size() && s1.substr(0, s2.size()) == s2;
}

template <typename T>
inline T roundup(T a, T b) {
#if 1
  return (a + b - 1) / b * b;
#else
  a += b - 1;
  return a - a % b;
#endif
}

template <typename T>
inline T coalesce(T a, T b) {
  return a ? a : b;
}

template <typename T>
inline T coalesce(T a, T b, T c) {
  return a ? a : b ? b : c;
}

class MarketBuffer : public std::vector<uint8_t> {
 public:
  TDF_MARKET_DATA* GetData() const { return (TDF_MARKET_DATA*)data(); }

  size_t GetSize() const { return size() / sizeof(TDF_MARKET_DATA); }
};

class OrderBuffer : public std::vector<uint8_t> {
 public:
  TDF_ORDER_X* GetData() const { return (TDF_ORDER_X*)data(); }

  size_t GetSize() const { return size() / sizeof(TDF_ORDER_X); }
};

class TransBuffer : public std::vector<uint8_t> {
 public:
  TDF_TRANSACTION_X* GetData() const { return (TDF_TRANSACTION_X*)data(); }

  size_t GetSize() const { return size() / sizeof(TDF_TRANSACTION_X); }
};

template <typename Compare>
class SemiBook {
 private:
  class Order {
   public:
    typedef std::unordered_map<int64_t, Order> Reg;
    typedef std::map<int64_t, int64_t, Compare> Levels;
    typedef std::unordered_set<Order*> OrderSet;

    class Set : private OrderSet {
     private:
      int64_t total;

     public:
      Set() : total(0) {}
      auto volume() const { return total; }
      auto size() const { return OrderSet::size(); }
      auto empty() const { return OrderSet::empty(); }
      auto begin() const { return OrderSet::begin(); }
      auto end() const { return OrderSet::end(); }
      auto insert(Order* p) {
        total += p->volume;
        return OrderSet::insert(p);
      }
      auto erase(Order* p) {
        total -= p->volume;
        return OrderSet::erase(p);
      }
    };

    typedef std::map<int64_t, Set, Compare> Cache;

   public:
    int64_t volume;
    typename Levels::iterator itl;
    typename Cache::iterator itc;
    bool active;
  };

 private:
  int64_t nTotalVolume = 0;  // 委托总量
  int64_t nTotalValue = 0;   // 委托总金额
  typename Order::Reg orders;
  typename Order::Levels levels;
  typename Order::Cache cache;
  typename Order::Levels::iterator delim = levels.end();
  int64_t iFrontFence = 0;
  int64_t iBackFence = 0;
  uint32_t nDepth = 0;

  Compare compare;

 public:
  void Reset() {
    nTotalVolume = 0;
    nTotalValue = 0;
    orders.clear();
    levels.clear();
    cache.clear();
    delim = levels.end();
    iFrontFence = 0;
    iBackFence = 0;
    nDepth = 0;
  }

  void SetFrontFence(int64_t price) {
    if (compare(price, iFrontFence)) {
      while (!cache.empty() && !compare(std::prev(cache.end())->first, price)) {
        auto itc = std::prev(cache.end());
        auto r = levels.emplace(itc->first, 0);
        auto itl = r.first;

        if (compare(iBackFence, itl->first)) {
          if (r.second &&
              (delim == levels.end() || compare(itl->first, delim->first)))
            delim = itl;
          for (auto p : itc->second) {
            itl->second += p->volume;
            p->itl = itl;
            p->itc = cache.end();
            p->active = true;
          }
          cache.erase(itc);
        } else {
          if (r.second) ++nDepth;
          for (auto p : itc->second) {
            itl->second += p->volume;
            p->itl = itl;
            p->itc = cache.end();
            p->active = true;
            nTotalVolume += p->volume;
            nTotalValue += itl->first * p->volume;
          }
          cache.erase(itc);
        }
      }
    }
    iFrontFence = price;
  }

  void SetBackFence(int64_t price) {
    if (compare(iBackFence, price)) {
      // Expand
      while (delim != levels.end() && !compare(price, delim->first)) {
        nTotalVolume += delim->second;
        nTotalValue += delim->first * delim->second;
        ++delim;
        ++nDepth;
      }
    } else if (compare(price, iBackFence)) {
      // Shrink
      while (levels.begin() != delim &&
             compare(price, std::prev(delim)->first)) {
        --delim;
        --nDepth;
        nTotalVolume -= delim->second;
        nTotalValue -= delim->first * delim->second;
      }
    }
    iBackFence = price;
  }

  void Add(int64_t id, int64_t price, int64_t volume) {
    if (price == 0) return;

    if (compare(price, iFrontFence)) {
      // Inactive
      auto itc = cache.emplace(price, typename Order::Set()).first;
      auto ro = orders.emplace(id, Order{volume, levels.end(), itc, false});

      if (ro.second) {
        itc->second.insert(&ro.first->second);
      } else {
        // Duplicate order id
        assert(!"Duplicate order id");
        if (itc->second.empty()) cache.erase(itc);
      }
    } else {
      // Active
      auto r = levels.emplace(price, 0);
      auto itl = r.first;
      auto ro = orders.emplace(id, Order{volume, itl, cache.end(), true});

      if (ro.second) {
        itl->second += volume;

        if (compare(iBackFence, price)) {
          // Active but hidden
          if (r.second &&
              (delim == levels.end() || compare(price, delim->first)))
            delim = itl;
        } else {
          // Visible
          if (r.second) ++nDepth;
          nTotalVolume += volume;
          nTotalValue += price * volume;
        }
      } else {
        // Duplicate order id
        assert(!"Duplicate order id");
        if (itl->second == 0) levels.erase(itl);
      }
    }
  }

  void Delete(int64_t id) {
    auto it = orders.find(id);

    if (it == orders.end()) return;

    if (it->second.active) {
      auto price = it->second.itl->first;
      auto volume = it->second.volume;

      if (compare(iBackFence, price)) {
        if (it->second.itl->second > volume)
          it->second.itl->second -= volume;
        else if (it->second.itl == delim)
          delim = levels.erase(it->second.itl);
        else
          levels.erase(it->second.itl);
        orders.erase(it);
      } else {
        if (it->second.itl->second > volume)
          it->second.itl->second -= volume;
        else {
          levels.erase(it->second.itl);
          --nDepth;
        }
        orders.erase(it);

        nTotalVolume -= volume;
        nTotalValue -= price * volume;
      }
    } else {
      if (it->second.itc->second.size() > 1)
        it->second.itc->second.erase(&it->second);
      else
        cache.erase(it->second.itc);
      orders.erase(it);
    }
  }

  void Trade(int64_t id, int64_t volume) {
    auto it = orders.find(id);

    if (it == orders.end()) return;

    if (!it->second.active) SetFrontFence(it->second.itc->first);

    auto price = it->second.itl->first;
    if (compare(iBackFence, price)) SetBackFence(price);

    if (it->second.volume > volume) {
      it->second.itl->second -= volume;
      it->second.volume -= volume;
    } else if (it->second.itl->second > volume) {
      it->second.itl->second -= volume;
      orders.erase(it);
    } else {
      levels.erase(it->second.itl);
      --nDepth;
      orders.erase(it);
    }

    nTotalVolume -= volume;
    nTotalValue -= price * volume;
  }

  int64_t TotalVolume() const { return nTotalVolume; }

  int64_t TotalValue() const { return nTotalValue; }

  uint32_t Depth() const { return nDepth; }

  auto Begin() const { return levels.begin(); }

  auto End() const { return delim; }

  int64_t BestPrice() const {
    return levels.begin() == delim ? 0 : levels.begin()->first;
  }

  uint32_t HeadDepth() const { return (uint32_t)cache.size(); }

  auto HeadBegin() const { return cache.begin(); }

  auto HeadEnd() const { return cache.end(); }

  uint32_t TailDepth() const { return (uint32_t)levels.size() - nDepth; }

  auto TailBegin() const { return delim; }

  auto TailEnd() const { return levels.end(); }
};

// 盘口变动类型
enum COMPARE_RESULT_TYPE { OB_UPDATE = 1, OB_INSERT = 2, OB_DELETE = 3 };

#define GO_UPDATE                                \
  if (showUnchanged || begNew[1] != begOld[1]) { \
    result[0] = begNew[0];                       \
    result[1] = OB_UPDATE;                       \
    result[2] = begNew[1] - begOld[1];           \
    result += 3;                                 \
  }                                              \
  begNew += 2;                                   \
  begOld += 2;

#define GO_INSERT        \
  result[0] = begNew[0]; \
  result[1] = OB_INSERT; \
  result[2] = begNew[1]; \
  result += 3;           \
  begNew += 2;

#define GO_DELETE         \
  result[0] = begOld[0];  \
  result[1] = OB_DELETE;  \
  result[2] = -begOld[1]; \
  result += 3;            \
  begOld += 2;

// 比较新旧盘口
template <typename Compare>
size_t OBCompare(const int64_t* begNew, size_t newSize, const int64_t* begOld,
                 size_t oldSize, int64_t* result, bool showUnchanged) {
  auto endNew = begNew + newSize;
  auto endOld = begOld + oldSize;
  auto begResult = result;

  while (true) {
    if (begNew < endNew) {
      if (begOld < endOld) {
        if (begNew[0] == begOld[0]) {
          GO_UPDATE;
        } else if (Compare()(begNew[0], begOld[0])) {
          GO_INSERT;
        } else {
          GO_DELETE;
        }
      } else {
        GO_INSERT;
        while (begNew < endNew) {
          GO_INSERT;
        }
        break;
      }
    } else if (begOld < endOld) {
      GO_DELETE;
      while (begOld < endOld) {
        GO_DELETE;
      }
      break;
    } else
      break;
  }

  return result - begResult;
}

#undef GO_UPDATE
#undef GO_INSERT
#undef GO_DELETE

class OrderBook {
 private:
  std::string windCode;     // 万得代码
  int32_t nTradingDay = 0;  // 交易日期
  int32_t nTime = 0;        // 交易所时间
  int32_t nNextTime = 0;    // 下一事件的交易所时间
  int64_t iSeqNum = 0;      // 顺序编号
  int64_t iVolume = 0;      // 成交总量
  int64_t iTurnover = 0;    // 成交总金额

  SemiBook<std::greater<int64_t>> buyBook;
  SemiBook<std::less<int64_t>> sellBook;

  OrderBuffer orderBuffer;
  TDF_ORDER_X* pOrder = nullptr;
  TDF_ORDER_X* pOrderEnd = nullptr;

  TransBuffer transBuffer;
  TDF_TRANSACTION_X* pTrans = nullptr;
  TDF_TRANSACTION_X* pTransEnd = nullptr;

  int64_t iLastPrice = 0;

  bool SSE = false;
  bool ChiNextRule = false;
  bool NoLimitsRule = false;

 private:
  void Reset() {
    windCode.clear();

    nTradingDay = 0;
    nTime = 0;
    nNextTime = 0;
    iSeqNum = 0;
    iVolume = 0;
    iTurnover = 0;

    buyBook.Reset();
    sellBook.Reset();

    orderBuffer.clear();
    pOrder = nullptr;
    pOrderEnd = nullptr;

    transBuffer.clear();
    pTrans = nullptr;
    pTransEnd = nullptr;

    iLastPrice = 0;

    SSE = false;
    ChiNextRule = false;
    NoLimitsRule = false;
  }

  void SSE_Order(const TDF_ORDER_X& order) {
#if 0
		std::cout << "SH order: " << order.nBizIndex << " "
			<< order.chOrderKind << order.chFunctionCode
			<< ", price=" << order.nPrice << ", vol=" << order.nVolume
			<< ", ori=" << order.nOrderOriNo << ", time=" << order.nTime << std::endl;
#endif
    assert(order.nTime >= nTime);
    assert(order.nBizIndex > iSeqNum);
    assert(order.chOrderKind == 'A' || order.chOrderKind == 'D');
    assert(order.chFunctionCode == 'B' || order.chFunctionCode == 'S');

    nTime = order.nTime;
    iSeqNum = order.nBizIndex;

    switch (order.chOrderKind) {
      case 'A':  // add order
        if (order.chFunctionCode == 'B')
          buyBook.Add(order.nOrderOriNo, order.nPrice, order.nVolume);
        else
          sellBook.Add(order.nOrderOriNo, order.nPrice, order.nVolume);
        break;

      case 'D':  // delete order
        if (order.chFunctionCode == 'B')
          buyBook.Delete(order.nOrderOriNo);
        else
          sellBook.Delete(order.nOrderOriNo);
        break;

      default:
        break;
    }
  }

  void SSE_Trans(const TDF_TRANSACTION_X& trans) {
#if 0
		std::cout << "SH trans: " << trans.nBizIndex
			<< ", price = " << trans.nPrice << ", vol = " << trans.nVolume
			<< ", buy=" << trans.nBidOrder << ", sell=" << trans.nAskOrder
			<< ", time=" << trans.nTime << std::endl;
#endif
    assert(trans.nTime >= nTime);
    assert(trans.nBizIndex > iSeqNum);

    nTime = trans.nTime;
    iSeqNum = trans.nBizIndex;

    iVolume += trans.nVolume;
    iTurnover += int64_t(trans.nPrice) * trans.nVolume;

    buyBook.Trade(trans.nBidOrder, trans.nVolume);
    sellBook.Trade(trans.nAskOrder, trans.nVolume);
    iLastPrice = trans.nPrice;
  }

  bool SSE_Step() {
    if (pOrder != pOrderEnd) {
      if (pTrans != pTransEnd) {
        if (pOrder->nBizIndex < pTrans->nBizIndex) {
          SSE_Order(*pOrder++);
          return true;
        } else {
          SSE_Trans(*pTrans++);
          return true;
        }
      } else {
        SSE_Order(*pOrder++);
        return true;
      }
    } else if (pTrans != pTransEnd) {
      SSE_Trans(*pTrans++);
      return true;
    } else
      return false;
  }

  void SZE_Order(const TDF_ORDER_X& order) {
#if 0
		std::cout << "SZ order: " << order.nOrder << " "
			<< order.chOrderKind << order.chFunctionCode
			<< ", price=" << order.nPrice << ", vol=" << order.nVolume
			<< ", time=" << order.nTime << std::endl;
#endif
    assert(order.nTime >= nTime);
    assert(order.nOrder > iSeqNum);
    assert(order.chOrderKind == '0' || order.chOrderKind == '1' ||
           order.chOrderKind == 'U');
    assert(order.chFunctionCode == 'B' || order.chFunctionCode == 'S');

    nTime = order.nTime;
    iSeqNum = order.nOrder;

    switch (order.chOrderKind) {
      case '0':  // 限价单
        if (order.chFunctionCode == 'B')
          buyBook.Add(order.nOrder, order.nPrice, order.nVolume);
        else
          sellBook.Add(order.nOrder, order.nPrice, order.nVolume);
        break;

      case '1':  // 市价单
        if (order.chFunctionCode == 'B')
          buyBook.Add(order.nOrder, sellBook.BestPrice(), order.nVolume);
        else
          sellBook.Add(order.nOrder, buyBook.BestPrice(), order.nVolume);
        break;

      case 'U':  // 本方最优
        if (order.chFunctionCode == 'B')
          buyBook.Add(order.nOrder, buyBook.BestPrice(), order.nVolume);
        else
          sellBook.Add(order.nOrder, sellBook.BestPrice(), order.nVolume);
        break;

      default:
        break;
    }
  }

  void SZE_Trans(const TDF_TRANSACTION_X& trans) {
#if 0
		std::cout << "SZ trans: " << trans.nIndex
			<< ", price = " << trans.nPrice << ", vol = " << trans.nVolume
			<< ", buy=" << trans.nBidOrder << ", sell=" << trans.nAskOrder
			<< ", time=" << trans.nTime << std::endl;
#endif
    assert(trans.nTime >= nTime);
    assert(trans.nIndex > iSeqNum);
    assert(trans.chFunctionCode == 'C' || trans.chFunctionCode == '0');

    nTime = trans.nTime;
    iSeqNum = trans.nIndex;

    if (trans.chFunctionCode == 'C')  // 撤单
    {
      if (trans.nBidOrder)
        buyBook.Delete(trans.nBidOrder);
      else
        sellBook.Delete(trans.nAskOrder);
    } else  // 成交
    {
      iVolume += trans.nVolume;
      iTurnover += int64_t(trans.nPrice) * trans.nVolume;
      buyBook.Trade(trans.nBidOrder, trans.nVolume);
      sellBook.Trade(trans.nAskOrder, trans.nVolume);
      iLastPrice = trans.nPrice;
    }
  }

  bool SZE_Step() {
    if (pOrder != pOrderEnd) {
      if (pTrans != pTransEnd) {
        if (pOrder->nOrder < pTrans->nIndex) {
          SZE_Order(*pOrder++);
          // 连续成交
          while (pTrans != pTransEnd && pTrans->nTime <= nNextTime &&
                 (pOrder == pOrderEnd || pTrans->nIndex < pOrder->nOrder) &&
                 pTrans->chFunctionCode != 'C')
            SZE_Trans(*pTrans++);
          return true;
        } else {
          SZE_Trans(*pTrans++);
          // 连续成交
          while (pTrans != pTransEnd && pTrans->nTime <= nNextTime &&
                 pTrans->nIndex < pOrder->nOrder &&
                 pTrans->chFunctionCode != 'C')
            SZE_Trans(*pTrans++);
          return true;
        }
      } else {
        SZE_Order(*pOrder++);
        return true;
      }
    } else if (pTrans != pTransEnd) {
      SZE_Trans(*pTrans++);
      // 连续成交
      while (pTrans != pTransEnd && pTrans->nTime <= nNextTime &&
             pTrans->chFunctionCode != 'C')
        SZE_Trans(*pTrans++);
      return true;
    } else
      return false;
  }

  // 正常股票有效竞价范围无特殊要求。
  void SetFenceDefault() {
    buyBook.SetFrontFence(INT64_MAX);
    buyBook.SetBackFence(0);
    sellBook.SetFrontFence(0);
    sellBook.SetBackFence(INT64_MAX);
  }

  // 无涨跌幅限制的股票开盘集合竞价的有效竞价范围为即时行情显示的前收盘价的900%以内。
  void SetFence900() {
    buyBook.SetFrontFence(iLastPrice * 9);
    buyBook.SetBackFence(0);
    sellBook.SetFrontFence(0);
    sellBook.SetBackFence(iLastPrice * 9);
  }

  // 无涨跌幅限制的股票连续竞价、盘中临时停牌复牌集合竞价、收盘集合竞价的有效竞价范围
  // 为最近成交价的上下10%。
  void SetFence10() {
    auto high =
        std::max((iLastPrice * 110 + 5000) / 10000 * 100, iLastPrice + 100);
    auto low =
        std::min((iLastPrice * 90 + 5000) / 10000 * 100, iLastPrice - 100);

    buyBook.SetFrontFence(high);
    buyBook.SetBackFence(low);

    sellBook.SetFrontFence(low);
    sellBook.SetBackFence(high);
  }

  // 创业板股票连续竞价阶段限价申报的有效竞价范围：
  //	（一）买入申报价格不得高于买入基准价格的102%；
  //	（二）卖出申报价格不得低于卖出基准价格的98%。
  void SetFence2() {
    auto buyRef =
        coalesce(sellBook.BestPrice(), buyBook.BestPrice(), iLastPrice);
    buyBook.SetFrontFence(
        std::max((buyRef * 102 + 5000) / 10000 * 100, buyRef + 100));
    buyBook.SetBackFence(0);

    auto sellRef =
        coalesce(buyBook.BestPrice(), sellBook.BestPrice(), iLastPrice);
    sellBook.SetFrontFence(
        std::min((sellRef * 98 + 5000) / 10000 * 100, sellRef - 100));
    sellBook.SetBackFence(INT64_MAX);
  }

 public:
  const std::string& WindCode() const { return windCode; }

  int TradingDay() const { return nTradingDay; }

  int Time() const { return nTime; }

  int NextTime() const { return nNextTime; }

  int64_t SeqNum() const { return iSeqNum; }

  int64_t LastPrice() const { return iLastPrice; }

  int64_t Volume() const { return iVolume; }

  int64_t Turnover() const { return (iTurnover + 5000) / 10000; }

  int64_t TotalBidVol() const { return buyBook.TotalVolume(); }

  int64_t TotalAskVol() const { return sellBook.TotalVolume(); }

  int64_t WeightedAvgBidPrice() const {
    return buyBook.TotalVolume() ? buyBook.TotalValue() / buyBook.TotalVolume()
                                 : 0;
  }

  int64_t WeightedAvgAskPrice() const {
    return sellBook.TotalVolume()
               ? sellBook.TotalValue() / sellBook.TotalVolume()
               : 0;
  }

  uint32_t BidDepth() const { return buyBook.Depth(); }

  auto BidBegin() const { return buyBook.Begin(); }

  auto BidEnd() const { return buyBook.End(); }

  uint32_t BidHeadDepth() const { return buyBook.HeadDepth(); }

  auto BidHeadBegin() const { return buyBook.HeadBegin(); }

  auto BidHeadEnd() const { return buyBook.HeadEnd(); }

  uint32_t BidTailDepth() const { return buyBook.TailDepth(); }

  auto BidTailBegin() const { return buyBook.TailBegin(); }

  auto BidTailEnd() const { return buyBook.TailEnd(); }

  uint32_t AskDepth() const { return sellBook.Depth(); }

  auto AskBegin() const { return sellBook.Begin(); }

  auto AskEnd() const { return sellBook.End(); }

  uint32_t AskHeadDepth() const { return sellBook.HeadDepth(); }

  auto AskHeadBegin() const { return sellBook.HeadBegin(); }

  auto AskHeadEnd() const { return sellBook.HeadEnd(); }

  uint32_t AskTailDepth() const { return sellBook.TailDepth(); }

  auto AskTailBegin() const { return sellBook.TailBegin(); }

  auto AskTailEnd() const { return sellBook.TailEnd(); }

  bool Load(XYData& reader, const std::string& code, int date) {
    Reset();
    if (reader.Read("OrderX", code, date, orderBuffer) == EC_OK &&
        orderBuffer.GetSize() > 0 &&
        reader.Read("TransX", code, date, transBuffer) == EC_OK &&
        transBuffer.GetSize() > 0) {
      windCode = code;
      nTradingDay = date;
      pOrder = orderBuffer.GetData();
      pOrderEnd = orderBuffer.GetData() + orderBuffer.GetSize();
      pTrans = transBuffer.GetData();
      pTransEnd = transBuffer.GetData() + transBuffer.GetSize();
      SSE = endswith(code, ".SH");
      ChiNextRule = date < NEW_RULES_DAY && !SSE && startswith(code, "30");

      if (date < NEW_RULES_DAY && !SSE) {
        MarketBuffer marketBuffer;
        if (reader.Read("Market", code, date, marketBuffer) == EC_OK &&
            marketBuffer.GetSize() > 0) {
#if 0
					for (size_t k = 0; k < marketBuffer.GetSize(); k++)
					{
						if (marketBuffer.GetData()[k].nStatus)
						{
							iLastPrice = marketBuffer.GetData()[k].nPreClose;
							NoLimitsRule = (marketBuffer.GetData()[k].nLowLimited <= 100);
							break;
						}
					}
#else
          auto k = marketBuffer.GetSize() - 1;
          iLastPrice = marketBuffer.GetData()[k].nPreClose;
          NoLimitsRule = (marketBuffer.GetData()[k].nLowLimited <= 100);
#endif
        } else {
          Reset();
          return false;
        }
      }

      if (NoLimitsRule)
        SetFence900();
      else
        SetFenceDefault();

      return true;
    } else {
      Reset();
      return false;
    }
  }

  // 单步执行一组不可分割的委托或成交。
  bool Step() {
    assert(nTime < MKT_START || nTime >= MKT_FINAL || BidBegin() == BidEnd() ||
           AskBegin() == AskEnd() || BidBegin()->first < AskBegin()->first);

    bool r = false;
    if (SSE)
      r = SSE_Step();
    else {
      if (nTime < MKT_START) {
        if (nNextTime >= MKT_START) {
          if (ChiNextRule)
            SetFence2();
          else if (NoLimitsRule)
            SetFence10();
        }
      } else if (nTime < MKT_FINAL) {
        if (nNextTime >= MKT_FINAL) {
          if (NoLimitsRule)
            SetFence10();
          else if (ChiNextRule)
            SetFenceDefault();
        }
      }

      r = SZE_Step();

      if (nTime >= MKT_START && nTime < MKT_FINAL) {
        if (ChiNextRule)
          SetFence2();
        else if (NoLimitsRule)
          SetFence10();
      }
    }

    if (pOrder != pOrderEnd) {
      if (pTrans != pTransEnd)
        nNextTime = std::min(pOrder->nTime, pTrans->nTime);
      else
        nNextTime = pOrder->nTime;
    } else {
      if (pTrans != pTransEnd)
        nNextTime = pTrans->nTime;
      else
        nNextTime = 240000000;  // end of day
    }

    return r;
  }

  // 运行至下一个快照。以period为周期，单位毫秒。
  bool Next(int period) {
    if (period <= 0) return Step();

    if (!Step()) return false;

    while (nNextTime <= roundup(nTime, period)) Step();

    return true;
  }

  bool is_SSE() { return SSE; }

  bool is_ChiNextRule() { return ChiNextRule; }

  bool is_NoLimitsRule() { return NoLimitsRule; }

  void Verify(const TDF_MARKET_DATA& market) const {
    if (Volume() != market.iVolume)
      std::cout << "Volume mismatch: " << Volume() << " " << market.iVolume
                << std::endl;

    if (Turnover() != market.iTurnover)
      std::cout << "Turnover mismatch: " << Turnover() << " "
                << market.iTurnover << std::endl;

    if (TotalBidVol() != market.nTotalBidVol)
      std::cout << "TotalBidVol mismatch: " << TotalBidVol() << " "
                << market.nTotalBidVol << std::endl;

    if (TotalAskVol() != market.nTotalAskVol)
      std::cout << "TotalAskVol mismatch: " << TotalAskVol() << " "
                << market.nTotalAskVol << std::endl;

    if (WeightedAvgBidPrice() != market.nWeightedAvgBidPrice)
      std::cout << "WeightedAvgBidPrice mismatch: " << WeightedAvgBidPrice()
                << " " << market.nWeightedAvgBidPrice << std::endl;

    if (WeightedAvgAskPrice() != market.nWeightedAvgAskPrice)
      std::cout << "WeightedAvgAskPrice mismatch: " << WeightedAvgAskPrice()
                << " " << market.nWeightedAvgAskPrice << std::endl;

    auto buy = buyBook.Begin();
    for (int k = 0; k < 10; k++) {
      if (buy != buyBook.End()) {
        if (buy->first != market.nBidPrice[k] ||
            buy->second != market.nBidVol[k]) {
          std::cout << "Buy mismatch: " << k << " (" << buy->first << ","
                    << buy->second << ") (" << market.nBidPrice[k] << ","
                    << market.nBidVol[k] << ")" << std::endl;
        }
        buy++;
      } else {
        if (market.nBidVol[k] != 0) {
          std::cout << "Buy mismatch: " << k << " (0,0) ("
                    << market.nBidPrice[k] << "," << market.nBidVol[k] << ")"
                    << std::endl;
        }
      }
    }

    auto sell = sellBook.Begin();
    for (int k = 0; k < 10; k++) {
      if (sell != sellBook.End()) {
        if (sell->first != market.nAskPrice[k] ||
            sell->second != market.nAskVol[k]) {
          std::cout << "Sell mismatch: " << k << " (" << sell->first << ","
                    << sell->second << ") (" << market.nAskPrice[k] << ","
                    << market.nAskVol[k] << ")" << std::endl;
        }
        sell++;
      } else {
        if (market.nAskVol[k] != 0) {
          std::cout << "Sell mismatch: " << k << " (0,0) ("
                    << market.nAskPrice[k] << "," << market.nAskVol[k] << ")"
                    << std::endl;
        }
      }
    }
  }

  bool Match(const TDF_MARKET_DATA& market) const {
    if (Volume() != market.iVolume || TotalBidVol() != market.nTotalBidVol ||
        TotalAskVol() != market.nTotalAskVol)
      return false;

    if (buyBook.Begin() == buyBook.End()) {
      if (market.nBidVol[0] != 0) return false;
    } else if (buyBook.Begin()->first != market.nBidPrice[0])
      return false;
    else if (buyBook.Begin()->second != market.nBidVol[0])
      return false;

    if (sellBook.Begin() == sellBook.End()) {
      if (market.nAskVol[0] != 0) return false;
    } else if (sellBook.Begin()->first != market.nAskPrice[0])
      return false;
    else if (sellBook.Begin()->second != market.nAskVol[0])
      return false;

    return true;
  }

  static size_t CompareBuys(const int64_t* begNew, size_t sizeNew,
                            const int64_t* begOld, size_t sizeOld,
                            int64_t* result, bool showUnchanged) {
    return OBCompare<std::greater<int64_t>>(begNew, sizeNew, begOld, sizeOld,
                                            result, showUnchanged);
  }

  static size_t CompareSells(const int64_t* begNew, size_t sizeNew,
                             const int64_t* begOld, size_t sizeOld,
                             int64_t* result, bool showUnchanged) {
    return OBCompare<std::less<int64_t>>(begNew, sizeNew, begOld, sizeOld,
                                         result, showUnchanged);
  }
};
