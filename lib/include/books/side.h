#pragma once

#include <cstdint>

namespace ff::common {
enum class Side : uint8_t
{
    Invalid = 0,
    Bid = 1,
    Ask
};

constexpr uint8_t get_side(Side side)
{
    return side == common::Side::Bid ? 0 : 1;
}
}  // namespace ff::common
