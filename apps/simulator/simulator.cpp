#include "protocol/request.h"
#include "tcp_server/coro_asio_server.h"
#include "tcp_server/orders_server.h"
using namespace std::chrono;

int main()
{
    auto ctx = asio::io_context{};
    ff::net::server::RequestsQueue requests_queue(2048);
    MDUpdatesQueue md_updates_queue{1 << 12};

    auto coro_server = ff::net::server::CoroAsioServer(ctx, 10004, requests_queue);
    ff::net::server::OrderServer order_server(requests_queue, md_updates_queue);

    std::cout << "Launching Order Gateway Service..." << std::endl;
    coro_server.run(2);
    return 0;
}