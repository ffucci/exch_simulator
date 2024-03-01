#include <boost/asio.hpp>
#include <deque>
#include <iostream>
#include <memory>
#include <string>

#include "tcp_server/asio_generic_server.h"

namespace ff::net::asio {
    // A shared pointer to itself control itself and acquire ownership of a
    // resource
    class ChatHandler : public std::enable_shared_from_this<ChatHandler> {
    public:
        ChatHandler(boost::asio::io_context& io_context)
            : io_context_(io_context),
              socket_(io_context),
              write_strand_(io_context) {}

        boost::asio::ip::tcp::socket& socket() { return socket_; }

        void read_next_packet() { read_packet(); }

        void send(std::string message) {
            io_context_.post(
                write_strand_.wrap([message, self = shared_from_this()]() {
                    self->queue_message(std::move(message));
                }));
        }

        virtual ~ChatHandler() = default;

    private:
        void read_packet() {
            boost::asio::async_read_until(
                socket_, in_packet_, '\n',
                [this, self = shared_from_this()](
                    auto error_code, size_t bytes_transferred) {
                    self->read_packet_done(error_code, bytes_transferred);
                });
        }

        void read_packet_done(
            const boost::system::error_code& ec, size_t bytes_transferred) {
            if (ec) {
                return;
            }

            std::istream stream(&in_packet_);
            std::string  packet_string;
            stream >> packet_string;
            std::cout << "Received packet: " << packet_string << std::endl;
            send(packet_string);
            read_next_packet();
        }

        void queue_message(std::string message) {
            bool is_write_in_progress = !send_packet_queue.empty();
            send_packet_queue.emplace_back(std::move(message));
            if (!is_write_in_progress) {
                start_packet_send();
            }
        }

        void start_packet_send() {
            // terminate the string
            std::cout << "Sending packet: " << send_packet_queue.front()
                      << std::endl;
            boost::asio::async_write(
                socket_, boost::asio::buffer(send_packet_queue.front()),
                write_strand_.wrap(
                    [self = shared_from_this()](auto ec, size_t _) {
                        self->packet_send_done(ec);
                    }));
        }

        void packet_send_done(const boost::system::error_code& ec) {
            if (ec) {
                return;
            }

            // Remove sent element
            send_packet_queue.pop_front();
            if (!send_packet_queue.empty()) {
                start_packet_send();
            }
        }

        boost::asio::io_context&     io_context_;
        boost::asio::ip::tcp::socket socket_;
        // Many threads can be writing to the same socket so we need a strand to
        // serialize the write to this socket
        boost::asio::io_context::strand write_strand_;
        boost::asio::streambuf          in_packet_;
        std::deque<std::string>         send_packet_queue;
    };

}  // namespace ff::net::asio

int main(void) {
    ff::net::asio::AsioGenericServer<ff::net::asio::ChatHandler> service(2);
    service.start_server(8888);

    return 0;
}