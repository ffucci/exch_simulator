#pragma once

#include <array>
#include <unordered_map>

#include "books/order.h"

namespace ff::books {

    class OrderBook {
    public:
        uint32_t add(const Order& order) {
            // see if we can match

            // add into the map
            return 0;
        }

        uint32_t modify(const Order& order) {
            // cancel and add
            return 0;
        }

        uint32_t cancel(const Order& order) { return 0; }

    private:
        static constexpr size_t BUFFER_SIZE = 3 * 1024 * 1024;

        std::array<OrderId, BUFFER_SIZE> bid_orders_{};
        std::array<OrderId, BUFFER_SIZE> ask_orders_{};
    };

}  // namespace ff::books