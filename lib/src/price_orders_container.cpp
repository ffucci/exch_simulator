#include "books/price_orders_container.h"

#include <iostream>

namespace ff::books {

    uint32_t PriceOrdersContainer::add(const Order& order) {
        return add_internal(order);
    }

    // INTERNAL METHODS
    uint32_t PriceOrdersContainer::add_internal(const Order& order) {
        const auto side    = common::get_side(order.side);
        auto& current_book = books_[side];
        auto found         = current_book.find(order.price);

        // add cumulative volume
        cumulative_volume_[side][order.price] += order.qty;
        if (found != current_book.end()) {
            auto& current_list = found->second;
            auto last_order_inserted =
                current_list.insert(current_list.end(), order);
            orders_to_iter_[order.order_id] = last_order_inserted;
            auto top_of_book                = (order.side == common::Side::Bid)
                                                  ? std::prev(current_book.end())
                                                  : std::begin(current_book);
            return (order.side == common::Side::Bid)
                       ? std::distance(found, std::prev(current_book.end()))
                       : std::distance(std::begin(current_book), found);
        }

        List<Order> new_level{};
        auto last_order_inserted = new_level.insert(new_level.end(), order);
        orders_to_iter_[order.order_id] = last_order_inserted;
        auto insertion_point = current_book.emplace(order.price, new_level);
        return (order.side == common::Side::Bid)
                   ? std::distance(
                         insertion_point.first, std::prev(current_book.end()))
                   : std::distance(
                         std::begin(current_book), insertion_point.first);
    }

    auto PriceOrdersContainer::orders(
        common::Side side, size_t index) const noexcept -> const List<Order>& {
        auto idx_side = common::get_side(side);
        auto pos      = books_[idx_side].cbegin();
        std::advance(pos, index);
        return pos->second;
    }

    auto PriceOrdersContainer::volume_for_price(common::Side side, Price price)
        const noexcept -> std::optional<Quantity> {
        auto idx_side                     = common::get_side(side);
        const auto& cumulative_volume_map = cumulative_volume_[idx_side];
        auto found                        = cumulative_volume_map.find(price);
        if (found == cumulative_volume_map.end()) {
            return std::nullopt;
        }

        return found->second;
    }
}  // namespace ff::books