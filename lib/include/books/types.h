#pragma once

#include <iostream>

#include <boost/lockfree/queue.hpp>
#include <boost/intrusive/list.hpp>

#include "books/order.h"

#include "absl/container/btree_map.h"
#include "protocol/update.h"

using UpdatesQueue = boost::lockfree::queue<ff::update::Update>;

namespace ff::books {

struct Order;
using OrderId = uint64_t;
using Price = int64_t;
using Quantity = uint32_t;
using InstrumentId = uint64_t;

template <typename T>
using List = boost::intrusive::list<T, boost::intrusive::constant_time_size<false>>;

using PriceOrderBook = absl::btree_map<Price, List<Order>>;
using PriceQuantityMap = absl::btree_map<Price, Quantity>;

struct Trade
{
    uint64_t aggressive_id{0};
    uint64_t passive_id{0};
    Price execution_price{0};
    Quantity execution_quantity{0};
};

inline std::ostream& operator<<(std::ostream& os, const Trade& trade)
{
    os << "aggressive_id: " << trade.aggressive_id << " - passive_id: " << trade.passive_id
       << " - execution_price: " << trade.execution_price << " - execution_qty: " << trade.execution_quantity;
    return os;
}
}  // namespace ff::books
