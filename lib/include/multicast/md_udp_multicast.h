#pragma once

#include <iostream>
#include <stop_token>
#include <string>
#include <boost/asio.hpp>
#include "books/types.h"
#include "protocol/update.h"

#include <thread>

constexpr short multicast_port = 30001;

namespace ff::net::md_multicast {

class MDUpdateMulticastSender
{
   public:
    MDUpdateMulticastSender(
        MDUpdatesQueue& md_updates_queue, boost::asio::io_context& io_context,
        const boost::asio::ip::address& multicast_address)
        : md_updates_queue_(md_updates_queue),
          endpoint_(multicast_address, multicast_port),
          socket_(io_context, endpoint_.protocol()),
          timer_(io_context),
          message_count_(0)
    {
        udp_thread_ = std::jthread([this](std::stop_token token) { do_send_update(); });
    }

   private:
    void do_send_update()
    {
        update::Update update;
        bool can_send = md_updates_queue_.pop(update);
        while (!can_send && md_updates_queue_.empty()) {
            can_send = md_updates_queue_.pop(update);
        }

        socket_.async_send_to(
            boost::asio::buffer(&update, sizeof(update)), endpoint_,
            [this](boost::system::error_code ec, std::size_t /*length*/) { do_send_update(); });
    }

   private:
    MDUpdatesQueue& md_updates_queue_;

    boost::asio::ip::udp::endpoint endpoint_;
    boost::asio::ip::udp::socket socket_;
    boost::asio::steady_timer timer_;
    int message_count_;
    std::string message_;

    std::jthread udp_thread_;
};

}  // namespace ff::net::md_multicast