#pragma once

#include <boost/fusion/include/define_struct.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/is_sequence.hpp>
#include <cstdint>
#include "books/types.h"

namespace ff::update {

enum class UpdateType : uint8_t
{
    Unknown = 0,
    Add,
    Cancel,
    Modify
};

}  // namespace ff::update

// clang-format off

BOOST_FUSION_DEFINE_STRUCT(
    (ff)(update), Update,
    (ff::update::UpdateType, type)
    (uint32_t, level)
    (int64_t, price)
    (uint32_t, quantity)
)

// clang-format on
