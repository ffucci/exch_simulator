#pragma once

#include <cstdlib>

#include "books/order.h"
#include "books/side.h"
#include "books/types.h"

namespace ff::books::tests {

    class OrderGenerator {
    public:
        OrderGenerator() = default;

        books::Order create_order(
            InstrumentId instrument_id, common::Side side, Price price,
            Quantity qty) {
            ++order_id_;
            books::Order order;
            order.side          = side;
            order.instrument_id = instrument_id;
            order.order_id      = order_id_;
            order.price         = price;
            order.qty           = qty;
            return order;
        }

    private:
        uint64_t order_id_{0};
    };
}  // namespace ff::books::tests