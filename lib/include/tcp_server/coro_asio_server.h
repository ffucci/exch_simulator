#pragma once

#include <atomic>
#include <iostream>
#include <span>
#include <thread>

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>

#include "books/order_book.h"
#include "books/types.h"
#include "protocol/request.h"
#include "protocol/update.h"

#include "ds/lockless_queue.h"

namespace asio = boost::asio;

using namespace std::chrono;
using boost::asio::ip::tcp;

namespace ff::net::server {
using OrderId = uint64_t;

struct SequencerData
{
    OrderId order_id{0};
    simulator::Request request;
};

// using RequestsQueue = ff::ds::LockFreeQueue<SequencerData>;
using RequestsQueue = boost::lockfree::queue<SequencerData>;

class FIFOSequencer
{
   public:
    explicit FIFOSequencer(RequestsQueue& orders_queue) : sequencer_queue_(orders_queue)
    {
    }

    void decode_and_sequence(std::span<const std::byte> buffer)
    {
        size_t current_offset{0};
        while (buffer.size() > 0) {
            SequencerData sequencer_request;
            std::memcpy(&sequencer_request.request, buffer.data(), sizeof(SequencerData::request));
            current_offset += sizeof(SequencerData::request);

            auto current_order_id = order_id_.fetch_add(1, std::memory_order::acq_rel);

            sequencer_request.order_id = current_order_id;
            std::cout << "Processing Request :  " << sequencer_request.order_id << " , "
                      << sequencer_request.request.to_string() << std::endl;

            sequencer_queue_.bounded_push(sequencer_request);
            buffer = buffer.subspan(current_offset);
        }
    }

   private:
    std::atomic<size_t> order_id_{1};
    RequestsQueue& sequencer_queue_;
};

class CoroAsioServer
{
   public:
    CoroAsioServer(asio::io_context& ctx, uint16_t port, RequestsQueue& requests_queue)
        : ctx_(ctx), port_(port), sequencer_(requests_queue)
    {
    }

    void run(uint8_t pool_size)
    {
        server_ = [this] {
            auto endpoint = tcp::endpoint{tcp::v4(), port_};
            auto awaitable = listen(endpoint);
            return awaitable;
        };
        asio::co_spawn(ctx_, server_, asio::detached);

        if (pool_size > 1) {
            threads.reserve(pool_size - 1);

            for (int i = 0; i < pool_size; ++i) {
                threads.emplace_back([this]() { ctx_.run(); });
            }
        }

        for (int i = 0; i < pool_size - 1; ++i) {
            threads[i].join();
        }
        ctx_.run();
    }

    auto listen(tcp::endpoint endpoint) -> asio::awaitable<void>
    {
        auto ex = co_await asio::this_coro::executor;
        auto a = tcp::acceptor{ex, std::move(endpoint)};

        while (true) {
            auto socket = co_await a.async_accept(asio::use_awaitable);
            auto session = [this, s = std::move(socket)]() mutable {
                auto awaitable = serve_client(std::move(s));
                return awaitable;
            };

            asio::co_spawn(ex, std::move(session), asio::detached);
        }
    }

   private:
    auto serve_client(tcp::socket socket) -> asio::awaitable<void>
    {
        std::cout << "New client connected from endpoint " << socket.remote_endpoint() << std::endl;
        auto ex = co_await asio::this_coro::executor;
        auto timer = asio::system_timer{ex};
        auto counter = 0;
        std::vector<std::byte> elements(BUFFER_SIZE, std::byte{0x00});

        while (true) {
            try {
                auto buf = boost::asio::buffer(elements);
                auto n = co_await socket.async_read_some(buf, asio::use_awaitable);
                // Decode request
                std::span<const std::byte> view_data{elements.data(), n};
                sequencer_.decode_and_sequence(view_data);
            } catch (const std::exception& e) {
                std::cerr << "Connection closed..." << '\n';
                break;
            }
        }
    }

    std::function<asio::awaitable<void>(void)> server_;
    asio::io_context& ctx_;
    std::vector<std::thread> threads;
    uint16_t port_;

    FIFOSequencer sequencer_;

    static constexpr size_t BUFFER_SIZE{1024};
};

}  // namespace ff::net::server