#pragma once

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <chrono>
#include <coroutine>
#include <deque>
#include <iostream>
#include <span>
#include <thread>

namespace asio = boost::asio;

using namespace std::chrono;
using boost::asio::ip::tcp;

namespace ff::net::server {

    template <typename Request>
    class Decoder {
    public:
        void decode(std::span<const std::byte> buffer) {
            size_t current_offset{0};
            while (buffer.size() > 0) {
                Request request{};
                std::memcpy(&request, buffer.data(), sizeof(Request));
                current_offset += sizeof(Request);
                std::cout << "Received request " << request.to_string()
                          << std::endl;
                buffer = buffer.subspan(current_offset);
            }
        }

    private:
    };

    template <typename Request>
    class CoroAsioServer {
    public:
        CoroAsioServer(asio::io_context& ctx, uint16_t port)
            : ctx_(ctx), port_(port) {}

        void run(uint8_t pool_size) {
            server_ = [this] {
                auto endpoint  = tcp::endpoint{tcp::v4(), port_};
                auto awaitable = listen(endpoint);
                return awaitable;
            };
            asio::co_spawn(ctx_, server_, asio::detached);

            if (pool_size > 1) {
                threads.resize(pool_size - 1);

                for (int i = 0; i < pool_size; ++i) {
                    threads[i] = std::thread([this]() { ctx_.run(); });
                }
            }

            for (int i = 0; i < pool_size - 1; ++i) {
                threads[i].join();
            }
            ctx_.run();
        }

        auto listen(tcp::endpoint endpoint) -> asio::awaitable<void> {
            auto ex = co_await asio::this_coro::executor;
            auto a  = tcp::acceptor{ex, std::move(endpoint)};

            while (true) {
                auto socket  = co_await a.async_accept(asio::use_awaitable);
                auto session = [this, s = std::move(socket)]() mutable {
                    auto awaitable = serve_client(std::move(s));
                    return awaitable;
                };

                asio::co_spawn(ex, std::move(session), asio::detached);
            }
        }

    private:
        auto serve_client(tcp::socket socket) -> asio::awaitable<void> {
            std::cout << "New client connected from endpoint "
                      << socket.remote_endpoint() << std::endl;
            auto ex      = co_await asio::this_coro::executor;
            auto timer   = asio::system_timer{ex};
            auto counter = 0;
            std::vector<std::byte> elements(1024, std::byte{0x00});

            while (true) {
                try {
                    auto buf = boost::asio::buffer(elements);
                    auto n   = co_await socket.async_read_some(
                        buf, asio::use_awaitable);
                    std::cout << "Read " << n << " byte(s)\n";
                    // Decode request
                    std::span<const std::byte> view_data{elements.data(), n};
                    decoder_.decode(view_data);
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

        Decoder<Request> decoder_;
        std::deque<Request> received_data_;
    };

}  // namespace ff::net::server