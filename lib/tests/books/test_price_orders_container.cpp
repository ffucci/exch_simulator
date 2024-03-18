#include <gtest/gtest.h>

#include "books/price_orders_container.h"

namespace ff::books::tests {

    class PriceOrdersContainerFixture : public ::testing::Test {
        void SetUp() override {}

    protected:
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
        // EXPECT_EQ(
        //     container.volume_for_price(common::Side::Bid, order2.price),
        //     std::nullopt);
    }
}  // namespace ff::books::tests
