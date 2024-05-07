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
    explicit OrderServer(ff::ds::sequencer::RequestsQueue& requests_queue, MDUpdatesQueue& md_updates);

   private:
    void process_request(const ds::sequencer::SequencerData& sequencer_request);

    ds::sequencer::RequestsQueue& requests_queue_;

    books::OrderBook order_book_;
    std::jthread polling_thread_;
};

}  // namespace ff::net::server