#pragma once

#include <cstdint>

#include "books/side.h"

namespace ff::books {

    using OrderId      = uint64_t;
    using Price        = int64_t;
    using Quantity     = uint32_t;
    using InstrumentId = uint64_t;

    struct Order {
        InstrumentId instrument_id{};
        OrderId order_id{};
        common::Side side;
        Price price{};
        Quantity qty{};

        auto operator<=>(const Order&) const = default;
    };

}  // namespace ff::books
