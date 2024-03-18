#pragma once

#include <plf_list.h>

#include <list>
#include <map>
#include <memory_resource>
#include <optional>
#include <type_traits>
#include <unordered_map>

#include "order.h"
#include "side.h"

namespace ff::books {

    // for PMR list need to go Linux
    template <typename T>
    using List = std::list<T>;

    using OrderToItr = std::unordered_map<OrderId, List<Order>::iterator>;

    using PriceOrderBook   = std::map<Price, List<Order>>;
    using PriceQuantityMap = std::map<Price, Quantity>;

    class PriceOrdersContainer {
    public:
        uint32_t add(const Order& order);

        auto cancel(const Order& order) -> uint32_t;

        void print() {}

        auto orders(common::Side side, size_t index) const noexcept
            -> const List<Order>&;

        auto volume_for_price(common::Side side, Price price) const noexcept
            -> std::optional<Quantity>;

    private:
        uint32_t add_internal(const Order& order);
        auto cancel_internal(const Order& order) -> uint32_t;

        static constexpr size_t NUM_BOOKS{2};
        PriceOrderBook books_[NUM_BOOKS];
        PriceQuantityMap cumulative_volume_[NUM_BOOKS];

        OrderToItr orders_to_iter_{};
    };

}  // namespace ff::books
