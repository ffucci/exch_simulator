#include "multicast/md_udp_multicast.h"
#include "protocol/request.h"
#include "tcp_server/coro_asio_server.h"
#include "tcp_server/orders_server.h"
using namespace std::chrono;

int main()
{
    auto ctx = asio::io_context{};
    ff::ds::sequencer::RequestsQueue requests_queue(2048);
    MDUpdatesQueue md_updates_queue{1 << 12};

    auto coro_server = ff::net::server::CoroAsioServer(ctx, 10004, requests_queue);
    ff::net::server::OrderServer order_server(requests_queue, md_updates_queue);
    ff::net::md_multicast::MDUpdateMulticastSender multicaster(
        md_updates_queue, ctx, boost::asio::ip::make_address("239.255.0.1"));

    std::cout << "Launching Order Gateway Service..." << std::endl;
    coro_server.run(2);
    return 0;
}