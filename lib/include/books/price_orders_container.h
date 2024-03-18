#pragma once

#include <plf_list.h>

#include <map>
#include <optional>
#include <type_traits>
#include <unordered_map>

#include "order.h"
#include "side.h"

namespace ff::books {

    template <typename T>
    using List = plf::list<T>;

    using OrderToItr = std::unordered_map<OrderId, List<Order>::iterator>;

    using PriceOrderBook   = std::map<Price, List<Order>>;
    using PriceQuantityMap = std::map<Price, Quantity>;

    class PriceOrdersContainer {
    public:
        uint32_t add(const Order& order);
        void print() {}

        auto orders(common::Side side, size_t index) const noexcept
            -> const List<Order>&;

        auto volume_for_price(common::Side side, Price price) const noexcept
            -> std::optional<Quantity>;

    private:
        uint32_t add_internal(const Order& order);

        static constexpr size_t NUM_BOOKS{2};
        PriceOrderBook books_[NUM_BOOKS];
        PriceQuantityMap cumulative_volume_[NUM_BOOKS];

        OrderToItr orders_to_iter_{};
    };

}  // namespace ff::books
