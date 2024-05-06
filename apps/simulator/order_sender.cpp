#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <chrono>
#include <coroutine>
#include <iostream>

#include "protocol/request.h"

namespace asio = boost::asio;
using namespace std::chrono;

using boost::asio::ip::tcp;

auto send_data(tcp::socket socket, int client_id) -> boost::asio::awaitable<void>
{
    constexpr size_t NUM_REQUESTS = 100;
    std::vector<std::byte> elements(sizeof(ff::simulator::Request), std::byte{0x00});

    ff::simulator::Request request;
    request.request_type = ff::simulator::RequestType::Add;
    request.client_id = client_id;
    request.instrument_id = 1345;
    request.price = 4000;
    request.quantity = 10;
    auto ex = co_await asio::this_coro::executor;
    auto timer = asio::system_timer{ex};
    int num_requests = 0;
    while (num_requests < NUM_REQUESTS) {
        std::memcpy(elements.data(), &request, elements.size());
        auto buf = asio::buffer(elements.data(), elements.size());
        request.price += 10;
        request.quantity += 10;
        auto n = co_await async_write(socket, buf, asio::use_awaitable);
        timer.expires_from_now(100ms);
        co_await timer.async_wait(asio::use_awaitable);
        num_requests++;
    }
}

int main(int argv, char** argc)
{
    auto ctx = asio::io_context{};
    if (argv < 2) {
        return 1;
    }

    auto client_id = std::stoi(std::string(argc[1]));

    auto client = [&ctx, client_id] {
        auto endpoint = tcp::endpoint{tcp::v4(), 10004};
        tcp::socket socket(ctx);

        std::cout << "Connecting to server at " << endpoint.address() << std::endl;
        socket.open(tcp::v4());
        socket.set_option(tcp::no_delay(true));

        socket.connect(endpoint);

        std::cout << "Connected to server..." << std::endl;
        return send_data(std::move(socket), client_id);
    };

    asio::co_spawn(ctx, std::move(client), asio::detached);
    ctx.run();
    return 0;
}