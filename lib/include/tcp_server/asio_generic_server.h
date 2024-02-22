#include <boost/asio.hpp>
#include <deque>
#include <iostream>
#include <memory>

namespace ff::net::asio {

    template <typename ConnectionHandler>
    class AsioGenericServer {
        using SharedHandlerT = std::shared_ptr<ConnectionHandler>;

    public:
        explicit AsioGenericServer(uint8_t thread_count)
            : thread_count_(thread_count), acceptor_(io_context_) {}

        void start_server(uint16_t port) {
            auto handler = std::make_shared<ConnectionHandler>(io_context_);

            boost::asio::ip::tcp::endpoint endpoint(
                boost::asio::ip::tcp::v4(), port);
            acceptor_.open(endpoint.protocol());

            acceptor_.set_option(
                boost::asio::ip::tcp::acceptor::reuse_address(true));

            acceptor_.bind(endpoint);

            std::cout << "Starting chat server on " << endpoint << std::endl;
            acceptor_.listen();

            acceptor_.async_accept(
                handler->socket(), [=, this](auto error_code) {
                    handle_new_connection(handler, error_code);
                });

            for (size_t i = 0; i < thread_count_; ++i) {
                thread_pool_.emplace_back([this] { io_context_.run(); });
            }

            for (size_t i = 0; i < thread_count_; ++i) {
                thread_pool_[i].join();
            }
        }

    private:
        void handle_new_connection(
            SharedHandlerT handler, const boost::system::error_code& ec) {
            if (ec) {
                return;
            }

            handler->read_next_packet();

            auto next_handler =
                std::make_shared<ConnectionHandler>(io_context_);

            acceptor_.async_accept(
                next_handler->socket(), [=, this](auto error_code) {
                    handle_new_connection(next_handler, error_code);
                });
        }

        uint8_t                        thread_count_;
        std::vector<std::thread>       thread_pool_;
        boost::asio::io_context        io_context_;
        boost::asio::ip::tcp::acceptor acceptor_;
    };
}  // namespace ff::net::asio