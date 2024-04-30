#include <gtest/gtest.h>

#include "books/price_orders_container.h"
#include "utility.h"

namespace ff::books::tests {

class PriceOrdersContainerFixture : public ::testing::Test
{
    void SetUp() override
    {
    }

   protected:
    OrderGenerator order_generator;
    books::PriceOrdersContainer container;
};

TEST_F(PriceOrdersContainerFixture, GIVEN_order_WHEN_add_on_ask_THEN_create_new_level)
{
    auto order = order_generator.create_order(1, common::Side::Ask, 1200, 10);
    auto order2 = order_generator.create_order(1, common::Side::Ask, 1300, 40);
    auto order3 = order_generator.create_order(1, common::Side::Ask, 1300, 70);

    auto level = container.add(order);
    EXPECT_EQ(level, 0);

    level = container.add(order2);
    EXPECT_EQ(level, 1);

    level = container.add(order3);
    EXPECT_EQ(level, 1);

    std::cout << container.to_string() << std::endl;
}

TEST_F(PriceOrdersContainerFixture, GIVEN_order_WHEN_add_on_bid_THEN_create_new_level)
{
    auto order = order_generator.create_order(1, common::Side::Bid, 1200, 10);
    auto order2 = order_generator.create_order(1, common::Side::Bid, 1100, 30);

    auto level = container.add(order);
    EXPECT_EQ(level, 0);

    level = container.add(order2);
    EXPECT_EQ(level, 1);

    std::cout << container.to_string() << std::endl;
}

TEST_F(PriceOrdersContainerFixture, GIVEN_order_WHEN_add_on_bid_THEN_orders_are_queued)
{
    auto order = order_generator.create_order(1, common::Side::Bid, 1200, 10);
    auto order2 = order_generator.create_order(1, common::Side::Bid, 1200, 30);

    auto level = container.add(order);
    EXPECT_EQ(level, 0);

    level = container.add(order2);
    EXPECT_EQ(level, 0);

    const auto& level_0 = container.orders(common::Side::Bid, 0);
    EXPECT_EQ(level_0.size(), 2);
    EXPECT_EQ(&level_0.front(), &order);
    EXPECT_EQ(&level_0.front(), &order);
}

TEST_F(PriceOrdersContainerFixture, GIVEN_order_WHEN_add_on_ask_THEN_orders_are_queued)
{
    auto order = order_generator.create_order(1, common::Side::Ask, 1200, 10);
    auto order2 = order_generator.create_order(1, common::Side::Ask, 1200, 30);
    auto order3 = order_generator.create_order(1, common::Side::Ask, 1500, 50);

    auto level = container.add(order);
    EXPECT_EQ(level, 0);

    level = container.add(order2);
    EXPECT_EQ(level, 0);

    level = container.add(order3);
    EXPECT_EQ(level, 1);

    const auto& level_0 = container.orders(common::Side::Ask, 0);
    EXPECT_EQ(level_0.size(), 2);
    EXPECT_EQ(&(*level_0.begin()), &order);

    EXPECT_EQ(container.volume_for_price(common::Side::Bid, 1100), std::nullopt);
    EXPECT_EQ(container.volume_for_price(common::Side::Ask, 1200), 40);

    std::cout << container.to_string() << std::endl;
}

TEST_F(PriceOrdersContainerFixture, GIVEN_order_WHEN_add_on_bid_and_cancel_THEN_level_is_removed)
{
    auto order = order_generator.create_order(1, common::Side::Bid, 1200, 10);
    auto order2 = order_generator.create_order(1, common::Side::Bid, 1100, 30);
    auto order3 = order_generator.create_order(1, common::Side::Bid, 1200, 130);

    auto level = container.add(order);
    EXPECT_EQ(level, 0);

    level = container.add(order2);
    EXPECT_EQ(level, 1);

    level = container.add(order3);
    EXPECT_EQ(level, 0);

    level = container.cancel(order2);
    EXPECT_EQ(level, 1);
    EXPECT_EQ(container.volume_for_price(common::Side::Bid, order2.price), std::nullopt);

    std::cout << container.to_string() << std::endl;
}

TEST_F(PriceOrdersContainerFixture, GIVEN_book_state_WHEN_cross_book_on_bid_THEN_trades_happen)
{
    auto order = order_generator.create_order(1, common::Side::Bid, 1200, 10);
    auto order2 = order_generator.create_order(1, common::Side::Ask, 1300, 30);
    auto order3 = order_generator.create_order(1, common::Side::Bid, 1300, 10);

    std::ignore = container.add(order);
    std::ignore = container.add(order2);

    EXPECT_EQ(container.volume_for_price(common::Side::Bid, order.price), 10);

    EXPECT_EQ(container.volume_for_price(common::Side::Ask, order2.price), 30);

    std::ignore = container.add(order3);
    EXPECT_EQ(container.volume_for_price(common::Side::Ask, order2.price), 20);

    auto order4 = order_generator.create_order(1, common::Side::Bid, 1350, 30);

    auto level = container.add(order4);
    EXPECT_EQ(level, 0);
    EXPECT_EQ(container.volume_for_price(common::Side::Ask, order2.price), std::nullopt);
    EXPECT_EQ(container.volume_for_price(common::Side::Bid, order4.price), 10);
}

TEST_F(PriceOrdersContainerFixture, GIVEN_book_state_WHEN_cross_book_on_bid_THEN_all_the_book_is_consumed)
{
    auto order = order_generator.create_order(1, common::Side::Bid, 1250, 50);
    auto order2 = order_generator.create_order(1, common::Side::Bid, 1200, 30);
    auto order3 = order_generator.create_order(1, common::Side::Ask, 1190, 100);

    auto level = container.add(order);
    EXPECT_EQ(level, 0);

    level = container.add(order2);
    EXPECT_EQ(level, 1);

    PriceOrdersContainer::Trades all_trades;
    std::invocable<PriceOrdersContainer::Trades> auto print_trades =
        [&all_trades](const PriceOrdersContainer::Trades& trades) {
            all_trades = trades;
            for (auto& trade : all_trades) {
                std::cout << trade << std::endl;
            }
        };

    level = container.add_with_match(order3, print_trades);
    EXPECT_EQ(level, 0);

    EXPECT_EQ(container.volume_for_price(common::Side::Ask, order3.price), 20);
    EXPECT_EQ(all_trades.size(), 2);
}

TEST_F(PriceOrdersContainerFixture, GIVEN_book_state_WHEN_big_aggressive_order_THEN_all_side_has_been_consumed)
{
    constexpr InstrumentId INSTRUMENT_ID{1};
    books::Order order = order_generator.create_order(INSTRUMENT_ID, common::Side::Ask, 1100, 20);

    auto order2 = order_generator.create_order(INSTRUMENT_ID, common::Side::Ask, 1200, 100);

    auto order3 = order_generator.create_order(INSTRUMENT_ID, common::Side::Ask, 1300, 80);

    auto crossing_order = order_generator.create_order(INSTRUMENT_ID, common::Side::Bid, 1300, 240);

    auto level = container.add(order);
    EXPECT_EQ(level, 0);

    level = container.add(order2);
    EXPECT_EQ(level, 1);

    level = container.add(order3);
    EXPECT_EQ(level, 2);

    PriceOrdersContainer::Trades all_trades;
    std::invocable<PriceOrdersContainer::Trades> auto print_trades =
        [&all_trades](const PriceOrdersContainer::Trades& trades) {
            all_trades = trades;
            for (auto& trade : all_trades) {
                std::cout << trade << std::endl;
            }
        };

    level = container.add_with_match(crossing_order, print_trades);
    EXPECT_EQ(level, 0);
    EXPECT_EQ(all_trades.size(), 3);
    EXPECT_EQ(container.volume_for_price(common::Side::Bid, crossing_order.price), 40);
}

}  // namespace ff::books::tests
