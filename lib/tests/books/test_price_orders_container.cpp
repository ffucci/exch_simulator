#include <gtest/gtest.h>

#include "books/price_orders_container.h"
#include "utility.h"

namespace ff::books::tests {

    class PriceOrdersContainerFixture : public ::testing::Test {
        void SetUp() override {}

    protected:
        OrderGenerator order_generator;
        books::PriceOrdersContainer container;
    };

    TEST_F(
        PriceOrdersContainerFixture,
        GIVEN_order_WHEN_add_on_ask_THEN_create_new_level) {
        books::Order order;
        order.order_id = 1;
        order.price    = 1200;
        order.qty      = 10;
        order.side     = common::Side::Ask;

        books::Order order2;
        order2.order_id = 1;
        order2.price    = 1300;
        order2.qty      = 10;
        order2.side     = common::Side::Ask;

        auto level = container.add(order);
        EXPECT_EQ(level, 0);

        level = container.add(order2);
        EXPECT_EQ(level, 1);
    }

    TEST_F(
        PriceOrdersContainerFixture,
        GIVEN_order_WHEN_add_on_bid_THEN_create_new_level) {
        books::Order order;
        order.order_id = 1;
        order.price    = 1200;
        order.qty      = 10;
        order.side     = common::Side::Bid;

        books::Order order2;
        order2.order_id = 2;
        order2.price    = 1100;
        order2.qty      = 30;
        order2.side     = common::Side::Bid;

        auto level = container.add(order);
        EXPECT_EQ(level, 0);

        level = container.add(order2);
        EXPECT_EQ(level, 1);
    }

    TEST_F(
        PriceOrdersContainerFixture,
        GIVEN_order_WHEN_add_on_bid_THEN_orders_are_queued) {
        books::Order order;
        order.order_id = 1;
        order.price    = 1200;
        order.qty      = 10;
        order.side     = common::Side::Bid;

        books::Order order2;
        order2.order_id = 2;
        order2.price    = 1200;
        order2.qty      = 30;
        order2.side     = common::Side::Bid;

        auto level = container.add(order);
        EXPECT_EQ(level, 0);

        level = container.add(order2);
        EXPECT_EQ(level, 0);

        const auto& level_0 = container.orders(common::Side::Bid, 0);
        EXPECT_EQ(level_0.size(), 2);
        EXPECT_EQ(*level_0.begin(), order);
    }

    TEST_F(
        PriceOrdersContainerFixture,
        GIVEN_order_WHEN_add_on_ask_THEN_orders_are_queued) {
        books::Order order;
        order.order_id = 1;
        order.price    = 1200;
        order.qty      = 10;
        order.side     = common::Side::Ask;

        books::Order order2;
        order2.order_id = 2;
        order2.price    = 1200;
        order2.qty      = 30;
        order2.side     = common::Side::Ask;

        books::Order order3;
        order3.order_id = 3;
        order3.price    = 1500;
        order3.qty      = 50;
        order3.side     = common::Side::Ask;

        auto level = container.add(order);
        EXPECT_EQ(level, 0);

        level = container.add(order2);
        EXPECT_EQ(level, 0);

        level = container.add(order3);
        EXPECT_EQ(level, 1);

        const auto& level_0 = container.orders(common::Side::Ask, 0);
        EXPECT_EQ(level_0.size(), 2);
        EXPECT_EQ(*level_0.begin(), order);

        EXPECT_EQ(
            container.volume_for_price(common::Side::Bid, 1100), std::nullopt);
        EXPECT_EQ(container.volume_for_price(common::Side::Ask, 1200), 40);
    }

    TEST_F(
        PriceOrdersContainerFixture,
        GIVEN_order_WHEN_add_on_bid_and_cancel_THEN_level_is_removed) {
        books::Order order;
        order.order_id = 1;
        order.price    = 1200;
        order.qty      = 10;
        order.side     = common::Side::Bid;

        books::Order order2;
        order2.order_id = 2;
        order2.price    = 1100;
        order2.qty      = 30;
        order2.side     = common::Side::Bid;

        books::Order order3;
        order3.order_id = 3;
        order3.price    = 1200;
        order3.qty      = 130;
        order3.side     = common::Side::Bid;

        auto level = container.add(order);
        EXPECT_EQ(level, 0);

        level = container.add(order2);
        EXPECT_EQ(level, 1);

        level = container.add(order3);
        EXPECT_EQ(level, 0);

        level = container.cancel(order2);
        EXPECT_EQ(level, 1);
        EXPECT_EQ(
            container.volume_for_price(common::Side::Bid, order2.price),
            std::nullopt);
    }

    TEST_F(
        PriceOrdersContainerFixture,
        GIVEN_book_state_WHEN_cross_book_on_bid_THEN_trades_happen) {
        books::Order order;
        order.order_id = 1;
        order.side     = common::Side::Bid;
        order.price    = 1200;
        order.qty      = 10;

        books::Order order2;
        order2.order_id = 2;
        order2.side     = common::Side::Ask;
        order2.price    = 1300;
        order2.qty      = 30;

        books::Order order3;
        order3.order_id = 3;
        order3.side     = common::Side::Bid;
        order3.price    = 1300;
        order3.qty      = 10;

        std::ignore = container.add(order);
        std::ignore = container.add(order2);

        EXPECT_EQ(
            container.volume_for_price(common::Side::Bid, order.price), 10);

        EXPECT_EQ(
            container.volume_for_price(common::Side::Ask, order2.price), 30);

        std::ignore = container.add(order3);
        EXPECT_EQ(
            container.volume_for_price(common::Side::Ask, order2.price), 20);

        books::Order order4;
        order4.order_id = 4;
        order4.side     = common::Side::Bid;
        order4.price    = 1350;
        order4.qty      = 30;

        auto level = container.add(order4);
        EXPECT_EQ(level, 0);
        EXPECT_EQ(
            container.volume_for_price(common::Side::Ask, order2.price),
            std::nullopt);
        EXPECT_EQ(
            container.volume_for_price(common::Side::Bid, order4.price), 10);
    }

    TEST_F(
        PriceOrdersContainerFixture,
        GIVEN_book_state_WHEN_cross_book_on_bid_THEN_all_the_book_is_consumed) {
        books::Order order;
        order.order_id = 1;
        order.side     = common::Side::Bid;
        order.price    = 1250;
        order.qty      = 50;

        books::Order order2;
        order2.order_id = 2;
        order2.side     = common::Side::Bid;
        order2.price    = 1200;
        order2.qty      = 30;

        books::Order order3;
        order3.order_id = 3;
        order3.side     = common::Side::Ask;
        order3.price    = 1190;
        order3.qty      = 100;

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

        EXPECT_EQ(
            container.volume_for_price(common::Side::Ask, order3.price), 20);
        EXPECT_EQ(all_trades.size(), 2);
    }

    TEST_F(
        PriceOrdersContainerFixture,
        GIVEN_book_state_WHEN_big_aggressive_order_THEN_all_side_has_been_consumed) {
        constexpr InstrumentId INSTRUMENT_ID{1};
        books::Order order = order_generator.create_order(
            INSTRUMENT_ID, common::Side::Ask, 1100, 20);

        auto order2 = order_generator.create_order(
            INSTRUMENT_ID, common::Side::Ask, 1200, 100);

        auto order3 = order_generator.create_order(
            INSTRUMENT_ID, common::Side::Ask, 1300, 80);

        auto crossing_order = order_generator.create_order(
            INSTRUMENT_ID, common::Side::Bid, 1300, 240);

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
        EXPECT_EQ(
            container.volume_for_price(common::Side::Bid, crossing_order.price),
            40);
    }

}  // namespace ff::books::tests
