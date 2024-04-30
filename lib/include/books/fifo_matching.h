#pragma once

#include "books/order.h"
#include "types.h"

#include <vector>

namespace ff::books {

class FifoMatching
{
   public:
    std::vector<Trade> match(Order& aggressive_order, PriceOrderBook& passive_book, PriceQuantityMap& cumulative_volume)
    {
        std::vector<Trade> trades;
        trades.reserve(1e5);
        // need to compare the top of the book
        while (aggressive_order.qty > 0 && !passive_book.empty() &&
               compare_with_passive(aggressive_order, passive_book.begin()->first)) {
            const auto current_price = passive_book.begin()->first;

            auto& top_row = passive_book.begin()->second;
            auto& next_order = top_row.front();
            auto traded_quantity = std::min(aggressive_order.qty, next_order.qty);

            trades.emplace_back(aggressive_order.order_id, next_order.order_id, current_price, traded_quantity);
            aggressive_order.qty -= traded_quantity;
            next_order.qty -= traded_quantity;

            if (next_order.qty == 0) {
                top_row.pop_front();
            }

            cumulative_volume[current_price] -= traded_quantity;
            if (cumulative_volume[current_price] == 0) {
                cumulative_volume.erase(current_price);
            }

            if (top_row.empty()) {
                passive_book.erase(current_price);
            }
        }
        return trades;
    }

   private:
    static constexpr auto compare_with_passive = [](const Order& aggressive_order, const Price passive_price) {
        if (aggressive_order.side == common::Side::Ask) {
            return aggressive_order.price <= passive_price;
        } else {
            return aggressive_order.price >= passive_price;
        }
    };
};

}  // namespace ff::books