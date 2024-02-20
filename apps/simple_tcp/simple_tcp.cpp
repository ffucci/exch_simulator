#include <iostream>
#include <thread>

#include <boost/asio.hpp>

namespace tcp = boost::asio::ip::tcp;

class Server
{
public:
    Server(boost::asio::io_context &io_context, short port) : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        handle_accept();
    }

private:
    void handle_accept()
    {
        acceptor_.async_accept([this](boost::system::error_code error_code, tcp::socket socket)
                               {
                                   if (error_code)
                                   {
                                       std::cout << "Cannot create session due to the following error: " << error_code.message() << std::endl;
                                       return;
                                   }

                                   std::cout << "Creating session coming from: " << socket.remote_endpoint(); });
        // waiting for the next accept
        handle_accept();
    }

    tcp::acceptor acceptor_;
}

void
handle_client(boost::asio::ip::tcp::socket &socket)
{
    boost::asio::streambuf buffer;
    boost::system::error_code error;

    boost::asio::read_until(socket, buffer, '\n', error);

    if (error)
    {
        std::cerr << "Error reading data from socket: " << error.message() << std::endl;
        return;
    }

    std::string message = boost::asio::buffer_cast<const char *>(buffer.data());
    boost::asio::write(socket, boost::asio::buffer(message), error);

    if (error)
    {
        std::cerr << "Error writing data: " << error.message() << std::endl;
    }
}

int main()
{
    boost::asio::io_context io_context;

    ip::tcp::acceptor acceptor(io_context, ip::tcp::endpoint(ip::tcp::v4(), 8888));

    std::cout << "Starting TCP server..." << std::endl;

    ip::tcp::socket socket(io_context);

    acceptor.accept(socket);

    std::cout << "New connection from: " << socket.remote_endpoint() << std::endl;

    while (true)
    {
        handle_client(socket);
    }
    return 0;
}