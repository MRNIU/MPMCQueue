// SPDX-License-Identifier: MIT
/**
 * @file threaded_example.cpp
 * @brief Multi-threaded example demonstrating MPMC functionality
 */

#include <mpmc_queue.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>

constexpr size_t QUEUE_CAPACITY = 256;
constexpr size_t NUM_PRODUCERS = 4;
constexpr size_t NUM_CONSUMERS = 4;
constexpr size_t ITEMS_PER_PRODUCER = 1000;

// Statistics
std::atomic<size_t> total_produced{0};
std::atomic<size_t> total_consumed{0};
std::atomic<size_t> failed_enqueues{0};

void producer(mpmc::MPMCQueue<int, QUEUE_CAPACITY>& queue, int producer_id) {
    size_t produced = 0;
    size_t failed = 0;
    
    for (size_t i = 0; i < ITEMS_PER_PRODUCER; ++i) {
        int value = producer_id * 10000 + static_cast<int>(i);
        
        // Retry until successful
        while (!queue.try_enqueue(value)) {
            ++failed;
            std::this_thread::yield();
        }
        ++produced;
    }
    
    total_produced.fetch_add(produced, std::memory_order_relaxed);
    failed_enqueues.fetch_add(failed, std::memory_order_relaxed);
}

void consumer(mpmc::MPMCQueue<int, QUEUE_CAPACITY>& queue, 
              std::atomic<bool>& done,
              [[maybe_unused]] int consumer_id) {
    size_t consumed = 0;
    int value;
    
    while (!done.load(std::memory_order_relaxed) || !queue.empty_approx()) {
        if (queue.try_dequeue(value)) {
            ++consumed;
        } else {
            std::this_thread::yield();
        }
    }
    
    total_consumed.fetch_add(consumed, std::memory_order_relaxed);
}

int main() {
    std::cout << "=== Multi-threaded MPMCQueue Example ===" << std::endl;
    std::cout << "Queue capacity: " << QUEUE_CAPACITY << std::endl;
    std::cout << "Number of producers: " << NUM_PRODUCERS << std::endl;
    std::cout << "Number of consumers: " << NUM_CONSUMERS << std::endl;
    std::cout << "Items per producer: " << ITEMS_PER_PRODUCER << std::endl;
    std::cout << "Total items to produce: " << (NUM_PRODUCERS * ITEMS_PER_PRODUCER) << std::endl;
    std::cout << std::endl;

    mpmc::MPMCQueue<int, QUEUE_CAPACITY> queue;
    std::atomic<bool> done{false};
    
    std::cout << "Starting test..." << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();

    // Start consumer threads
    std::vector<std::thread> consumers;
    for (size_t i = 0; i < NUM_CONSUMERS; ++i) {
        consumers.emplace_back(consumer, std::ref(queue), std::ref(done), i);
    }

    // Start producer threads
    std::vector<std::thread> producers;
    for (size_t i = 0; i < NUM_PRODUCERS; ++i) {
        producers.emplace_back(producer, std::ref(queue), i);
    }

    // Wait for all producers to finish
    for (auto& thread : producers) {
        thread.join();
    }
    
    std::cout << "All producers finished" << std::endl;
    
    // Signal consumers to stop after queue is empty
    done.store(true, std::memory_order_relaxed);
    
    // Wait for all consumers to finish
    for (auto& thread : consumers) {
        thread.join();
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << std::endl;
    std::cout << "=== Results ===" << std::endl;
    std::cout << "Total produced: " << total_produced.load() << std::endl;
    std::cout << "Total consumed: " << total_consumed.load() << std::endl;
    std::cout << "Failed enqueue attempts: " << failed_enqueues.load() << std::endl;
    std::cout << "Final queue size (approx): " << queue.size_approx() << std::endl;
    std::cout << "Time elapsed: " << duration.count() << " ms" << std::endl;
    
    if (total_produced.load() == total_consumed.load() && 
        total_produced.load() == NUM_PRODUCERS * ITEMS_PER_PRODUCER) {
        std::cout << std::endl;
        std::cout << "✓ SUCCESS: All items were produced and consumed correctly!" << std::endl;
        return 0;
    } else {
        std::cout << std::endl;
        std::cout << "✗ ERROR: Mismatch in produced/consumed items!" << std::endl;
        return 1;
    }
}
