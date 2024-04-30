#include <gtest/gtest.h>
#include <cstddef>

#include "books/order.h"
#include "books/order_book.h"
#include "books/side.h"
#include "utility.h"

namespace ff::books::tests {

class OrderBookFixture : public ::testing::Test
{
    void SetUp() override
    {
    }

   protected:
    OrderBook order_book;
    OrderGenerator order_generator;
};

TEST_F(OrderBookFixture, GIVEN_order_book_WHEN_add_multiple_adds_THEN_orders_are_added)
{
    auto order = order_generator.create_order(1, common::Side::Ask, 1200, 10);
    auto order2 = order_generator.create_order(1, common::Side::Ask, 1300, 40);

    auto level = order_book.add(std::move(order));
    EXPECT_EQ(level, 0);

    level = order_book.add(std::move(order2));
    EXPECT_EQ(level, 1);

    EXPECT_EQ(order_book.number_active_orders(), 2);
}

TEST_F(OrderBookFixture, GIVEN_order_book_WHEN_add_multiple_and_cancel_THEN_orders_are_added)
{
    constexpr size_t NUM_ORDERS{100};
    for (size_t idx = 0; idx < NUM_ORDERS; ++idx) {
        auto order = order_generator.create_order(1, common::Side::Ask, 1200 + 10 * (idx % 5), 10 + idx);
        std::ignore = order_book.add(std::move(order));
    }

    std::ignore = order_book.cancel(NUM_ORDERS / 2);
    EXPECT_EQ(order_book.number_active_orders(), NUM_ORDERS - 1);

    std::ignore = order_book.cancel((NUM_ORDERS / 2) - 1);
    EXPECT_EQ(order_book.number_active_orders(), NUM_ORDERS - 2);
}
}  // namespace ff::books::tests
