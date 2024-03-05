#pragma once

#include <cstdint>

namespace ff::simulator {

    using ClientId     = uint64_t;
    using InstrumentId = uint64_t;
    using Price        = uint64_t;
    using Quantity     = uint32_t;

    enum class RequestType : uint8_t { Invalid = 0, Add, Cancel, Modify };

    enum class Side : uint8_t { Invalid = 0, Bid = 1, Ask };

#pragma pack(push, 1)
    struct Request {
        RequestType request_type{RequestType::Invalid};
        ClientId client_id{0};
        InstrumentId instrument_id{0};
        Side side{Side::Invalid};
        Price price{0};
        Quantity quantity{0};
        std::string to_string() const noexcept {
            std::stringstream ss;
            ss << "(request_type: " << static_cast<int>(request_type)
               << " - client_id: " << client_id
               << " - instrument_id: " << instrument_id << " - price: " << price
               << " - qty: " << quantity << " )";
            return ss.str();
        }
    };

#pragma pack(pop)
}  // namespace ff::simulator
