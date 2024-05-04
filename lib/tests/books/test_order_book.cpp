#include <gtest/gtest.h>
#include <boost/fusion/algorithm/iteration/for_each_fwd.hpp>
#include <cstddef>

#include "books/order.h"
#include "books/order_book.h"
#include "books/side.h"
#include "books/types.h"
#include "protocol/update.h"
#include "utility.h"

namespace ff::books::tests {

struct print
{
    template <typename T>
    void operator()(const T &t) const
    {
        std::cout << static_cast<uint32_t>(t) << '\n';
    }
};

class OrderBookFixture : public ::testing::Test
{
    void SetUp() override
    {
    }

   protected:
    static constexpr size_t SIZE_PAGE{1 << 10};
    UpdatesQueue md_updates{SIZE_PAGE};
    OrderBook order_book{md_updates};
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
    update::Update result;
    EXPECT_TRUE(md_updates.pop(result));
    EXPECT_EQ(result.type, update::UpdateType::Add);
    EXPECT_EQ(result.level, 0);
    EXPECT_TRUE(md_updates.pop(result));
    EXPECT_EQ(result.type, update::UpdateType::Add);
    EXPECT_EQ(result.level, 1);

    boost::fusion::for_each(result, print{});
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

TEST_F(OrderBookFixture, GIVEN_order_book_WHEN_add_multiple_and_cancel_THEN_trades_happen)
{
    auto order = order_generator.create_order(1, common::Side::Ask, 1200, 50);
    auto order2 = order_generator.create_order(1, common::Side::Ask, 1300, 70);
    auto order3 = order_generator.create_order(1, common::Side::Ask, 1320, 90);

    std::ignore = order_book.add(std::move(order));
    std::ignore = order_book.add(std::move(order2));
    std::ignore = order_book.add(std::move(order3));

    auto crossing_order = order_generator.create_order(1, common::Side::Bid, 1250, 70);
    std::ignore = order_book.add(std::move(crossing_order));

    std::cout << order_book.to_string() << std::endl;
    for (auto trade : order_book.get_trades()) {
        std::cout << trade << std::endl;
    }

    EXPECT_EQ(order_book.get_trades().size(), 1);
    auto crossing_order_2 = order_generator.create_order(1, common::Side::Bid, 1370, 100);
    std::ignore = order_book.add(std::move(crossing_order_2));

    std::cout << order_book.to_string() << std::endl;
    for (auto trade : order_book.get_trades()) {
        std::cout << trade << std::endl;
    }
}
}  // namespace ff::books::tests
