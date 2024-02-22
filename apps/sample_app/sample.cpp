#include <boost/asio.hpp>
#include <iostream>
#include <string>

class Printer {
public:
    explicit Printer(boost::asio::io_context& io_context, size_t num_calls)
        : strand_(boost::asio::make_strand(io_context)),
          timer1_(io_context, boost::asio::chrono::seconds(1)),
          timer2_(io_context, boost::asio::chrono::seconds(2)),
          count_(0),
          num_calls_(num_calls) {
        timer1_.async_wait(boost::asio::bind_executor(
            strand_,
            [this](const boost::system::error_code& ec) { handler1(); }));

        timer2_.async_wait(boost::asio::bind_executor(
            strand_,
            [this](const boost::system::error_code& ec) { handler2(); }));
    }

    ~Printer() { std::cout << "Final count is: " << count_ << std::endl; }

private:
    void handler1() {
        if (count_ < num_calls_) {
            std::cout << "Timer 1 called # " << count_ << " times."
                      << std::endl;
            ++count_;
            timer1_.expires_at(
                timer1_.expiry() + boost::asio::chrono::seconds(1));
            timer1_.async_wait(boost::asio::bind_executor(
                strand_,
                [this](const boost::system::error_code& ec) { handler1(); }));
        }
    }

    void handler2() {
        if (count_ < num_calls_) {
            std::cout << "Timer 2 called # " << count_ << " times."
                      << std::endl;
            ++count_;
            timer2_.expires_at(
                timer2_.expiry() + boost::asio::chrono::seconds(2));
            timer2_.async_wait(boost::asio::bind_executor(
                strand_,
                [this](const boost::system::error_code& ec) { handler2(); }));
        }
    }

    boost::asio::strand<boost::asio::io_context::executor_type> strand_;
    boost::asio::steady_timer                                   timer1_;
    boost::asio::steady_timer                                   timer2_;
    size_t                                                      count_;
    size_t                                                      num_calls_;
};

int main(void) {
    boost::asio::io_context io_context;
    Printer                 printer(io_context, 20);

    std::thread t([&io_context]() { io_context.run(); });
    io_context.run();
    t.join();
    return 0;
}