#pragma once

#include "ds/lockless_queue.h"

namespace ff::ds {

    template <typename T, size_t SIZE>
    void LockFreeQueue<T, SIZE>::enqueue(const T& data) {
        auto write_index = write_index_.fetch_add(1, std::memory_order_acquire);
        elements_[write_index] = std::move(data);
        write_index_           = (write_index + 1) & (SIZE - 1);
    }

    template <typename T, size_t SIZE>
    auto LockFreeQueue<T, SIZE>::pop() -> const T* {
        auto read_index = read_index_.fetch_add(1, std::memory_order_acquire);
        read_index      = (read_index + 1) & (SIZE - 1);
        return &elements_[read_index];
    }

    template <typename T, size_t SIZE>
    bool LockFreeQueue<T, SIZE>::empty() const noexcept {
        return read_index_ == write_index_;
    }

    template <typename T, size_t SIZE>
    bool LockFreeQueue<T, SIZE>::full() const noexcept {
        return read_index_ == (write_index_ + 1);
    }
}