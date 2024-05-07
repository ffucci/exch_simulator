#pragma once

#include "books/types.h"
#include "protocol/request.h"

namespace ff::ds::sequencer {

struct SequencerData
{
    books::OrderId order_id{0};
    simulator::Request request;
};

using RequestsQueue = boost::lockfree::queue<SequencerData>;

class FIFOSequencer
{
   public:
    explicit FIFOSequencer(RequestsQueue& orders_queue) : sequencer_queue_(orders_queue)
    {
    }

    void decode_and_sequence(std::span<const std::byte> buffer)
    {
        size_t current_offset{0};
        while (buffer.size() > 0) {
            SequencerData sequencer_request;
            std::memcpy(&sequencer_request.request, buffer.data(), sizeof(SequencerData::request));
            current_offset += sizeof(SequencerData::request);

            auto current_order_id = order_id_.fetch_add(1, std::memory_order::acq_rel);

            sequencer_request.order_id = current_order_id;
            std::cout << "Processing Request :  " << sequencer_request.order_id << " , "
                      << sequencer_request.request.to_string() << std::endl;

            sequencer_queue_.bounded_push(sequencer_request);
            buffer = buffer.subspan(current_offset);
        }
    }

   private:
    std::atomic<size_t> order_id_{1};
    RequestsQueue& sequencer_queue_;
};

}  // namespace ff::ds::sequencer