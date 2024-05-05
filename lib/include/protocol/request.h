#pragma once

#include <cstdint>
#include <sstream>

#include "books/side.h"
#include "books/types.h"

namespace ff::simulator {

using ClientId = uint64_t;

enum class RequestType : uint8_t
{
    Invalid = 0,
    Add,
    Cancel,
    Modify
};

#pragma pack(push, 1)
struct Request
{
    RequestType request_type{RequestType::Invalid};
    ClientId client_id{0};
    books::InstrumentId instrument_id{0};
    common::Side side{common::Side::Invalid};
    books::Price price{0};
    books::Quantity quantity{0};
    std::string to_string() const noexcept
    {
        std::stringstream ss;
        ss << "(request_type: " << static_cast<int>(request_type) << " - client_id: " << client_id
           << " - instrument_id: " << instrument_id << " - price: " << price << " - qty: " << quantity << " )";
        return ss.str();
    }
};

#pragma pack(pop)
}  // namespace ff::simulator
