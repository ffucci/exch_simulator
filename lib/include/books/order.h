#pragma once

#include <cstdint>

#include "books/side.h"
#include "types.h"

namespace ff::books {

    struct Order {
        InstrumentId instrument_id{};
        OrderId order_id{};
        common::Side side;
        Price price{};
        Quantity qty{};

        auto operator<=>(const Order&) const = default;
    };

}  // namespace ff::books
