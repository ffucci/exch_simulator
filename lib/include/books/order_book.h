#pragma once

#include <cstdint>
#include <string>

#include "books/order.h"
#include "books/price_orders_container.h"
#include "books/types.h"
#include "protocol/update.h"

#include <boost/lockfree/queue.hpp>

namespace ff::books {

class OrderBook
{
   public:
    OrderBook(MDUpdatesQueue& updates) : updates_(updates)
    {
        orders_.reserve(1 << 20);
        all_trades.reserve(1 << 14);
    }

    [[nodiscard]] auto add(Order&& order) noexcept -> std::optional<uint32_t>;

    uint32_t modify(OrderId order_id, Price old_price, Price new_price, Quantity old_qty, Quantity qty)
    {
        // We can create a specific modify
        return 0;
    }

    auto cancel(OrderId order_id) -> uint32_t;

    auto get_trades() const noexcept -> const PriceOrdersContainer::Trades&
    {
        return all_trades;
    }

    [[nodiscard]] auto number_active_orders() const noexcept -> uint64_t;

    [[nodiscard]] auto to_string() const noexcept -> std::string
    {
        return price_orders.to_string();
    }

   private:
    std::vector<Order> orders_{};
    PriceOrdersContainer price_orders{};
    PriceOrdersContainer::Trades all_trades;

    MDUpdatesQueue& updates_;
};

}  // namespace ff::books