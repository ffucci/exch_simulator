#include "books/price_orders_container.h"

namespace ff::books {

    template <std::invocable<PriceOrdersContainer::Trades> OnMatch>
    auto PriceOrdersContainer::add_with_match(
        Order& order, OnMatch&& on_match_handler) -> std::optional<uint32_t> {
        const auto passive_side = common::get_side(order.side) ^
                                  0x1;  // bid == 0 => ask, ask == 1 => bid == 0

        auto& passive_book              = books_[passive_side];
        auto& passive_cumulative_volume = cumulative_volume_[passive_side];

        auto trades = matching_strategy.match(
            order, passive_book, passive_cumulative_volume);

        if (on_match_handler) {
            on_match_handler(trades);
        }

        if (order.qty == 0) {
            return std::nullopt;
        }
        return add_internal(order);
    }
}