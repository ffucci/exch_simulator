#include "books/price_orders_container.h"

namespace ff::books {

auto PriceOrdersContainer::add(Order& order) noexcept -> std::optional<uint32_t>
{
    std::invocable<Trades> auto dummy_handler = [](const Trades& trades) {};
    return add_with_match(order, std::move(dummy_handler));
}

auto PriceOrdersContainer::cancel(const Order& order) -> uint32_t
{
    return cancel_internal(order);
}

// INTERNAL METHODS
uint32_t PriceOrdersContainer::add_internal(Order& order) noexcept
{
    const auto side = common::get_side(order.side);
    auto& current_book = books_[side];
    auto found = current_book.find(order.price);

    // add cumulative volume
    cumulative_volume_[side][order.price] += order.qty;
    if (found != current_book.end()) {
        auto& current_list = found->second;
        auto last_order_inserted = current_list.insert(current_list.end(), order);
        auto top_of_book = (order.side == common::Side::Bid) ? std::prev(current_book.end()) : std::begin(current_book);
        return (order.side == common::Side::Bid) ? std::distance(found, std::prev(current_book.end()))
                                                 : std::distance(std::begin(current_book), found);
    }

    List<Order> new_level{};
    new_level.push_back(order);
    auto insertion_point = current_book.emplace(order.price, std::move(new_level));
    return (order.side == common::Side::Bid) ? std::distance(insertion_point.first, std::prev(current_book.end()))
                                             : std::distance(std::begin(current_book), insertion_point.first);
}

auto PriceOrdersContainer::cancel_internal(const Order& order) -> uint32_t
{
    const auto side = common::get_side(order.side);
    auto& current_book = books_[side];
    auto found = current_book.find(order.price);

    auto& current_list = found->second;
    // add cumulative volume
    cumulative_volume_[side][order.price] -= order.qty;
    if (cumulative_volume_[side][order.price] == 0) {
        cumulative_volume_[side].erase(order.price);
    }
    auto removed_distance = (order.side == common::Side::Bid) ? std::distance(found, std::prev(current_book.end()))
                                                              : std::distance(std::begin(current_book), found);
    current_list.erase(current_list.iterator_to(order));
    if (current_list.empty()) {
        current_book.erase(order.price);
    }
    return removed_distance;
}

auto PriceOrdersContainer::orders(common::Side side, size_t index) const noexcept -> const List<Order>&
{
    auto idx_side = common::get_side(side);
    auto pos = books_[idx_side].cbegin();
    std::advance(pos, index);
    return pos->second;
}

auto PriceOrdersContainer::volume_for_price(common::Side side, Price price) const noexcept -> std::optional<Quantity>
{
    auto idx_side = common::get_side(side);
    const auto& cumulative_volume_map = cumulative_volume_[idx_side];
    auto found = cumulative_volume_map.find(price);
    if (found == cumulative_volume_map.end()) {
        return std::nullopt;
    }

    return found->second;
}
}  // namespace ff::books