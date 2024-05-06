#pragma once

#include <stop_token>
#include <thread>
#include "books/order_book.h"
#include "books/types.h"
#include "protocol/request.h"
#include "tcp_server/coro_asio_server.h"
namespace ff::net::server {

class OrderServer
{
   public:
    explicit OrderServer(RequestsQueue& requests_queue);

   private:
    void process_request(const SequencerData& sequencer_request);

    UpdatesQueue md_updates_{1 << 12};
    RequestsQueue& requests_queue_;

    books::OrderBook order_book_{md_updates_};
    std::jthread polling_thread_;
};

}  // namespace ff::net::server