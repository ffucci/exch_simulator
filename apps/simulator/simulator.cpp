#include "request.h"
#include "tcp_server/coro_asio_server.h"
using namespace std::chrono;

int main() {
    auto ctx = asio::io_context{};
    auto coro_server =
        ff::net::server::CoroAsioServer<ff::simulator::Request>(ctx, 37259);

    std::cout << "Launching Order Gateway Service..." << std::endl;
    coro_server.run(4);
    return 0;
}