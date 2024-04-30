#pragma once

#include <cstddef>
#include <cstdint>
#include <ios>
#include "books/order.h"

#include "absl/container/flat_hash_map.h"
#include "books/price_orders_container.h"
#include "books/side.h"
#include "books/types.h"

namespace ff::books {

class OrderBook
{
   public:
    OrderBook()
    {
        orders_.reserve(1 << 20);
    }
    [[nodiscard]] auto add(Order&& order) noexcept -> std::optional<uint32_t>;
    uint32_t modify(const Order& order)
    {
        // cancel and add
        return 0;
    }

    auto cancel(OrderId order_id) -> uint32_t;

    [[nodiscard]] auto number_active_orders() const noexcept -> uint64_t;

   private:
    std::vector<Order> orders_{};
    PriceOrdersContainer price_orders{};
};

}  // namespace ff::books