#include "books/order_book.h"
#include <algorithm>
#include <bit>
#include "books/order.h"

namespace ff::books {

auto OrderBook::add(Order&& order) noexcept -> std::optional<uint32_t>
{
    // see if we can match
    orders_.emplace_back(std::move(order));
    // add into the map
    return price_orders.add(orders_.back());
}

auto OrderBook::number_active_orders() const noexcept -> uint64_t
{
    const auto number_active_orders =
        std::count_if(std::begin(orders_), std::end(orders_), [](const auto& order) { return order.qty > 0; });
    return number_active_orders;
}

auto OrderBook::cancel(OrderId order_id) -> uint32_t
{
    auto& order = orders_.at(order_id);
    const auto update_level = price_orders.cancel(order);
    orders_[order_id].qty = 0;
    return update_level;
}
}  // namespace ff::books