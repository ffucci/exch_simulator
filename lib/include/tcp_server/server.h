#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <span>

namespace ip = boost::asio::ip;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(ip::tcp::socket socket) : m_socket(std::move(socket)) {}

    void run() { wait_for_next_request(); }

private:
    void wait_for_next_request() {
        auto self(shared_from_this());
        boost::asio::async_read_until(
            m_socket, m_buffer, '\n',
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (ec) {
                    std::cout << "error: " << ec.message() << std::endl;
                    return;
                }
                std::string data{
                    std::istreambuf_iterator<char>(&m_buffer),
                    std::istreambuf_iterator<char>()};
                auto bytes_data =
                    std::as_bytes(std::span{data.data(), data.size()});
                std::cout << bytes_data.size() << std::endl;
                for (auto b : bytes_data) {
                    std::cout << std::hex << static_cast<int>(b) << " ";
                }
                std::cout << std::endl;
                std::cout << "length received data: " << length << std::endl;
                wait_for_next_request();
            });
    }

    ip::tcp::socket        m_socket;
    boost::asio::streambuf m_buffer;
};

class Server {
public:
    Server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, ip::tcp::endpoint(ip::tcp::v4(), port)) {
        std::cout << "Starting TCP server on port " << port << " ..."
                  << std::endl;
        handle_accept();
    }

private:
    void handle_accept() {
        acceptor_.async_accept(
            [this](
                boost::system::error_code error_code, ip::tcp::socket socket) {
                if (error_code) {
                    std::cout
                        << "Cannot create session due to the following error: "
                        << error_code.message() << std::endl;
                    return;
                }

                std::cout << "Creating session coming from: "
                          << socket.remote_endpoint() << std::endl;
                std::make_shared<Session>(std::move(socket))->run();
                // waiting for the next accept
                handle_accept();
            });
    }

    ip::tcp::acceptor acceptor_;
};