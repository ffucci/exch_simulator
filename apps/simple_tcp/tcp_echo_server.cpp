#include <boost/asio.hpp>
#include <iostream>
#include <memory>

#include "tcp_server/server.h"

namespace ip = boost::asio::ip;

int main() {
    boost::asio::io_context io_context;
    Server server(io_context, 8888);
    io_context.run();  // run the proactor
    return 0;
}