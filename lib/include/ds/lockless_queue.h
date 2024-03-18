#pragma once

#include <array>
#include <atomic>
#include <cstdint>
#include <utility>

namespace ff::ds {

    template <typename T, size_t SIZE = 1024>
    class LockFreeQueue {
    public:
        void enqueue(const T& data);

        auto pop() -> const T*;

        bool empty() const noexcept;

        bool full() const noexcept;

    private:
        std::atomic<size_t> write_index_{0};
        std::atomic<size_t> read_index_{0};
        std::array<T, SIZE> elements_{};
    };

}  // namespace ff::ds

#include "ds/lockless_queue.hpp"