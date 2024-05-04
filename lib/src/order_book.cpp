#include <algorithm>

#include "books/order_book.h"
#include "books/order.h"
#include "protocol/update.h"

namespace ff::books {

auto OrderBook::add(Order&& order) noexcept -> std::optional<uint32_t>
{
    std::invocable<PriceOrdersContainer::Trades> auto handler = [this](const PriceOrdersContainer::Trades& trades) {
        std::move(trades.begin(), trades.end(), std::back_inserter(all_trades));
    };
    auto current_price = order.price;
    auto current_qty = order.qty;
    orders_.emplace_back(std::move(order));
    auto update_level = price_orders.add_with_match(orders_.back(), std::move(handler));
    if (update_level) {
        updates_.bounded_push(update::Update(update::UpdateType::Add, *update_level, current_price, current_qty));
    }
    return update_level;
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
    updates_.bounded_push(update::Update{update::UpdateType::Cancel, update_level, order.price, order.qty});
    return update_level;
}
}  // namespace ff::books