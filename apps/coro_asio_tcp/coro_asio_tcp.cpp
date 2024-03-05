#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <chrono>
#include <coroutine>
#include <iostream>
#include <thread>
#include <variant>

using namespace std::chrono;
namespace asio = boost::asio;

using boost::asio::ip::tcp;

auto serve_client(tcp::socket socket) -> asio::awaitable<void> {
    std::cout << "New client connected\n";
    auto ex      = co_await asio::this_coro::executor;
    auto timer   = asio::system_timer{ex};
    auto counter = 0;
    while (true) {
        try {
            auto s   = std::to_string(counter) + '\n';
            auto buf = asio::buffer(s.data(), s.size());
            auto n   = co_await async_write(socket, buf, asio::use_awaitable);
            std::cout << "Wrote " << n << " byte(s)\n";
            ++counter;
            timer.expires_from_now(500ms);
            co_await timer.async_wait(asio::use_awaitable);
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            break;
        }
    }
}

auto listen(tcp::endpoint endpoint) -> asio::awaitable<void> {
    auto ex = co_await asio::this_coro::executor;
    auto a  = tcp::acceptor{ex, std::move(endpoint)};

    while (true) {
        auto socket  = co_await a.async_accept(asio::use_awaitable);
        auto session = [s = std::move(socket)]() mutable {
            auto awaitable = serve_client(std::move(s));
            return awaitable;
        };

        asio::co_spawn(ex, std::move(session), asio::detached);
    }
}

int main() {
    auto server = [] {
        auto endpoint  = tcp::endpoint{tcp::v4(), 37259};
        auto awaitable = listen(endpoint);
        return awaitable;
    };
    auto ctx = asio::io_context{};
    asio::co_spawn(ctx, server, asio::detached);

    std::vector<std::thread> threads(3);

    for (int i = 0; i < 3; ++i) {
        threads[i] = std::thread([&ctx]() { ctx.run(); });
    }

    for (int i = 0; i < 3; ++i) {
        threads[i].join();
    }
    ctx.run();
    return 0;
}