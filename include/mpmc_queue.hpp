// SPDX-License-Identifier: MIT
/**
 * @file mpmc_queue.hpp
 * @brief Multi-Producer Multi-Consumer Lock-Free Queue
 * 
 * A lock-free MPMC queue implementation for freestanding C++26 environments.
 * Uses atomic operations and a ring buffer for thread-safe, wait-free operations.
 */

#ifndef MPMC_QUEUE_HPP
#define MPMC_QUEUE_HPP

#include <atomic>
#include <cstddef>
#include <cstdint>

namespace mpmc {

/**
 * @brief Multi-Producer Multi-Consumer Lock-Free Queue
 * 
 * This implementation uses a ring buffer with atomic operations to provide
 * lock-free enqueue and dequeue operations. It's suitable for freestanding
 * environments as it doesn't use dynamic memory allocation, exceptions, or
 * other standard library facilities beyond atomics.
 * 
 * @tparam T The type of elements stored in the queue
 * @tparam Capacity The maximum number of elements (must be power of 2)
 */
template <typename T, size_t Capacity>
class MPMCQueue {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of 2");
    static_assert(Capacity > 0, "Capacity must be greater than 0");

public:
    /**
     * @brief Construct a new MPMCQueue object
     */
    constexpr MPMCQueue() noexcept : head_(0), tail_(0) {
        for (size_t i = 0; i < Capacity; ++i) {
            buffer_[i].sequence.store(i, std::memory_order_relaxed);
        }
    }

    /**
     * @brief Destroy the MPMCQueue object
     */
    ~MPMCQueue() noexcept = default;

    // Non-copyable and non-movable
    MPMCQueue(const MPMCQueue&) = delete;
    MPMCQueue& operator=(const MPMCQueue&) = delete;
    MPMCQueue(MPMCQueue&&) = delete;
    MPMCQueue& operator=(MPMCQueue&&) = delete;

    /**
     * @brief Attempt to enqueue an item
     * 
     * @param item The item to enqueue
     * @return true if the item was successfully enqueued
     * @return false if the queue is full
     */
    [[nodiscard]] bool try_enqueue(const T& item) noexcept {
        return enqueue_impl(item);
    }

    /**
     * @brief Attempt to enqueue an item (move version)
     * 
     * @param item The item to enqueue
     * @return true if the item was successfully enqueued
     * @return false if the queue is full
     */
    [[nodiscard]] bool try_enqueue(T&& item) noexcept {
        return enqueue_impl(static_cast<T&&>(item));
    }

    /**
     * @brief Attempt to dequeue an item
     * 
     * @param item Reference to store the dequeued item
     * @return true if an item was successfully dequeued
     * @return false if the queue is empty
     */
    [[nodiscard]] bool try_dequeue(T& item) noexcept {
        size_t pos;
        Cell* cell;
        size_t seq;

        pos = tail_.load(std::memory_order_relaxed);
        
        for (;;) {
            cell = &buffer_[pos & (Capacity - 1)];
            seq = cell->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos + 1);

            if (diff == 0) {
                if (tail_.compare_exchange_weak(pos, pos + 1, 
                                                std::memory_order_relaxed)) {
                    item = static_cast<T&&>(cell->data);
                    cell->sequence.store(pos + Capacity, std::memory_order_release);
                    return true;
                }
            } else if (diff < 0) {
                return false;
            } else {
                pos = tail_.load(std::memory_order_relaxed);
            }
        }
    }

    /**
     * @brief Get the capacity of the queue
     * 
     * @return constexpr size_t The maximum number of elements
     */
    [[nodiscard]] static constexpr size_t capacity() noexcept {
        return Capacity;
    }

    /**
     * @brief Get an approximate size of the queue
     * 
     * Note: This is an approximation and may not be accurate in concurrent scenarios.
     * 
     * @return size_t Approximate number of elements in the queue
     */
    [[nodiscard]] size_t size_approx() const noexcept {
        size_t head = head_.load(std::memory_order_relaxed);
        size_t tail = tail_.load(std::memory_order_relaxed);
        return head >= tail ? head - tail : 0;
    }

    /**
     * @brief Check if the queue is empty (approximate)
     * 
     * Note: This is an approximation and may not be accurate in concurrent scenarios.
     * 
     * @return true if the queue appears to be empty
     * @return false if the queue appears to have elements
     */
    [[nodiscard]] bool empty_approx() const noexcept {
        return size_approx() == 0;
    }

private:
    struct Cell {
        std::atomic<size_t> sequence;
        T data;
    };

    template <typename U>
    [[nodiscard]] bool enqueue_impl(U&& item) noexcept {
        size_t pos;
        Cell* cell;
        size_t seq;

        pos = head_.load(std::memory_order_relaxed);
        
        for (;;) {
            cell = &buffer_[pos & (Capacity - 1)];
            seq = cell->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos);

            if (diff == 0) {
                if (head_.compare_exchange_weak(pos, pos + 1, 
                                                std::memory_order_relaxed)) {
                    cell->data = static_cast<U&&>(item);
                    cell->sequence.store(pos + 1, std::memory_order_release);
                    return true;
                }
            } else if (diff < 0) {
                return false;
            } else {
                pos = head_.load(std::memory_order_relaxed);
            }
        }
    }

    // Cache line padding to avoid false sharing
    static constexpr size_t kCacheLineSize = 64;
    
    alignas(kCacheLineSize) std::atomic<size_t> head_;
    alignas(kCacheLineSize) std::atomic<size_t> tail_;
    
    // Ring buffer
    alignas(kCacheLineSize) Cell buffer_[Capacity];
};

} // namespace mpmc

#endif // MPMC_QUEUE_HPP
