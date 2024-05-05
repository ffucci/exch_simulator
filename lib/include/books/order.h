#pragma once

#include "books/side.h"
#include "types.h"

namespace ff::books {

using mode = boost::intrusive::link_mode<boost::intrusive::auto_unlink>;

struct Order : public boost::intrusive::list_base_hook<mode>
{
    InstrumentId instrument_id{};
    OrderId order_id{};
    common::Side side;
    Price price{};
    Quantity qty{};
};

inline std::ostream& operator<<(std::ostream& os, const Order& order)
{
    os << "( " << order.order_id << "," << order.price << "," << order.qty << " )";
    return os;
}

}  // namespace ff::books
