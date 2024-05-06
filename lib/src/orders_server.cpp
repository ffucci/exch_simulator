#include "tcp_server/orders_server.h"
#include "books/types.h"

namespace ff::net::server {

OrderServer::OrderServer(RequestsQueue& requests_queue, MDUpdatesQueue& md_updates)
    : requests_queue_(requests_queue), order_book_(md_updates)
{
    polling_thread_ = std::jthread([this](std::stop_token token) {
        while (!token.stop_requested()) {
            SequencerData sequencer_request;
            auto can_read = requests_queue_.pop(sequencer_request);
            if (can_read) {
                process_request(sequencer_request);
            }
            std::this_thread::yield();
        }
    });
}

void OrderServer::process_request(const SequencerData& sequencer_request)
{
    auto& request = sequencer_request.request;
    switch (request.request_type) {
        case simulator::RequestType::Add: {
            books::Order order;
            order.instrument_id = request.instrument_id;
            order.order_id = sequencer_request.order_id;
            order.side = request.side;
            order.price = request.price;
            order.qty = request.quantity;
            std::ignore = order_book_.add(std::move(order));
            std::cout << order << std::endl;
        } break;

        default:
            std::cerr << "Unrecognized response..." << std::endl;
            break;
    }
}
}  // namespace ff::net::server