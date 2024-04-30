#pragma once

#include <concepts>
#include <list>
#include <map>
#include <memory_resource>
#include <optional>
#include <type_traits>
#include <unordered_map>

#include "books/fifo_matching.h"
#include "books/order.h"
#include "books/side.h"
#include "books/types.h"

namespace ff::books {

class PriceOrdersContainer
{
   public:
    using Trades = std::vector<Trade>;

    auto add(Order& order) noexcept -> std::optional<uint32_t>;

    template <std::invocable<PriceOrdersContainer::Trades> OnMatch>
    auto add_with_match(Order& order, OnMatch&& on_match_handler) noexcept -> std::optional<uint32_t>;

    auto cancel(const Order& order) -> uint32_t;

    auto orders(common::Side side, size_t index) const noexcept -> const List<Order>&;

    auto volume_for_price(common::Side side, Price price) const noexcept -> std::optional<Quantity>;

    auto to_string() const noexcept
    {
        std::stringstream ss;
        for (auto& ladder : books_) {
            for (auto& [price, v] : ladder) {
                ss << price << "-> ";
                for (auto& el : v) {
                    ss << el << " ";
                }
                ss << std::endl;
            }
        }

        return ss.str();
    }

   private:
    uint32_t add_internal(Order& order) noexcept;
    auto cancel_internal(const Order& order) -> uint32_t;

    static constexpr size_t NUM_BOOKS{2};
    PriceOrderBook books_[NUM_BOOKS];
    PriceQuantityMap cumulative_volume_[NUM_BOOKS];

    FifoMatching matching_strategy{};
};

}  // namespace ff::books

#include "books/price_orders_container.hpp"