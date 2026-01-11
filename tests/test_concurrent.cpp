// SPDX-License-Identifier: MIT
/**
 * @file test_concurrent.cpp
 * @brief Concurrent operations tests for MPMCQueue
 */

#include <mpmc_queue.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <algorithm>

#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "FAIL: " << message << std::endl; \
            std::cerr << "  at " << __FILE__ << ":" << __LINE__ << std::endl; \
            return 1; \
        } \
    } while (0)

#define TEST_PASS(message) \
    std::cout << "PASS: " << message << std::endl

int main() {
    std::cout << "=== Running Concurrent Operations Tests ===" << std::endl << std::endl;

    // Test 1: Single producer, single consumer
    {
        mpmc::MPMCQueue<int, 256> queue;
        std::atomic<bool> done{false};
        constexpr size_t NUM_ITEMS = 10000;
        
        std::thread producer([&]() {
            for (size_t i = 0; i < NUM_ITEMS; ++i) {
                while (!queue.try_enqueue(static_cast<int>(i))) {
                    std::this_thread::yield();
                }
            }
        });
        
        std::thread consumer([&]() {
            size_t count = 0;
            int value;
            while (count < NUM_ITEMS) {
                if (queue.try_dequeue(value)) {
                    TEST_ASSERT(value == static_cast<int>(count), 
                               "Value should match expected sequence");
                    ++count;
                } else {
                    std::this_thread::yield();
                }
            }
        });
        
        producer.join();
        consumer.join();
        
        TEST_ASSERT(queue.empty_approx(), "Queue should be empty after test");
        TEST_PASS("Single producer, single consumer");
    }

    // Test 2: Multiple producers, single consumer
    {
        mpmc::MPMCQueue<int, 512> queue;
        constexpr size_t NUM_PRODUCERS = 4;
        constexpr size_t ITEMS_PER_PRODUCER = 1000;
        std::atomic<size_t> produced_count{0};
        std::atomic<size_t> consumed_count{0};
        
        std::vector<std::thread> producers;
        for (size_t p = 0; p < NUM_PRODUCERS; ++p) {
            producers.emplace_back([&, p]() {
                for (size_t i = 0; i < ITEMS_PER_PRODUCER; ++i) {
                    int value = static_cast<int>(p * ITEMS_PER_PRODUCER + i);
                    while (!queue.try_enqueue(value)) {
                        std::this_thread::yield();
                    }
                    produced_count.fetch_add(1, std::memory_order_relaxed);
                }
            });
        }
        
        std::thread consumer([&]() {
            int value;
            while (consumed_count.load(std::memory_order_relaxed) < 
                   NUM_PRODUCERS * ITEMS_PER_PRODUCER) {
                if (queue.try_dequeue(value)) {
                    consumed_count.fetch_add(1, std::memory_order_relaxed);
                } else {
                    std::this_thread::yield();
                }
            }
        });
        
        for (auto& t : producers) {
            t.join();
        }
        consumer.join();
        
        TEST_ASSERT(produced_count.load() == NUM_PRODUCERS * ITEMS_PER_PRODUCER,
                   "All items should be produced");
        TEST_ASSERT(consumed_count.load() == NUM_PRODUCERS * ITEMS_PER_PRODUCER,
                   "All items should be consumed");
        TEST_PASS("Multiple producers, single consumer");
    }

    // Test 3: Single producer, multiple consumers
    {
        mpmc::MPMCQueue<int, 512> queue;
        constexpr size_t NUM_CONSUMERS = 4;
        constexpr size_t TOTAL_ITEMS = 10000;
        std::atomic<size_t> consumed_count{0};
        
        std::thread producer([&]() {
            for (size_t i = 0; i < TOTAL_ITEMS; ++i) {
                while (!queue.try_enqueue(static_cast<int>(i))) {
                    std::this_thread::yield();
                }
            }
        });
        
        std::vector<std::thread> consumers;
        for (size_t c = 0; c < NUM_CONSUMERS; ++c) {
            consumers.emplace_back([&]() {
                int value;
                while (consumed_count.load(std::memory_order_relaxed) < TOTAL_ITEMS) {
                    if (queue.try_dequeue(value)) {
                        consumed_count.fetch_add(1, std::memory_order_relaxed);
                    } else {
                        std::this_thread::yield();
                    }
                }
            });
        }
        
        producer.join();
        for (auto& t : consumers) {
            t.join();
        }
        
        TEST_ASSERT(consumed_count.load() == TOTAL_ITEMS,
                   "All items should be consumed");
        TEST_PASS("Single producer, multiple consumers");
    }

    // Test 4: Multiple producers, multiple consumers
    {
        mpmc::MPMCQueue<int, 1024> queue;
        constexpr size_t NUM_PRODUCERS = 4;
        constexpr size_t NUM_CONSUMERS = 4;
        constexpr size_t ITEMS_PER_PRODUCER = 2500;
        std::atomic<size_t> produced_count{0};
        std::atomic<size_t> consumed_count{0};
        std::atomic<bool> producers_done{false};
        
        std::vector<std::thread> producers;
        for (size_t p = 0; p < NUM_PRODUCERS; ++p) {
            producers.emplace_back([&]() {
                for (size_t i = 0; i < ITEMS_PER_PRODUCER; ++i) {
                    while (!queue.try_enqueue(static_cast<int>(i))) {
                        std::this_thread::yield();
                    }
                    produced_count.fetch_add(1, std::memory_order_relaxed);
                }
            });
        }
        
        std::vector<std::thread> consumers;
        for (size_t c = 0; c < NUM_CONSUMERS; ++c) {
            consumers.emplace_back([&]() {
                int value;
                while (!producers_done.load(std::memory_order_relaxed) || 
                       !queue.empty_approx()) {
                    if (queue.try_dequeue(value)) {
                        consumed_count.fetch_add(1, std::memory_order_relaxed);
                    } else {
                        std::this_thread::yield();
                    }
                }
            });
        }
        
        for (auto& t : producers) {
            t.join();
        }
        producers_done.store(true, std::memory_order_relaxed);
        
        for (auto& t : consumers) {
            t.join();
        }
        
        TEST_ASSERT(produced_count.load() == NUM_PRODUCERS * ITEMS_PER_PRODUCER,
                   "All items should be produced");
        TEST_ASSERT(consumed_count.load() == NUM_PRODUCERS * ITEMS_PER_PRODUCER,
                   "All items should be consumed");
        TEST_ASSERT(queue.empty_approx(), "Queue should be empty after test");
        TEST_PASS("Multiple producers, multiple consumers");
    }

    std::cout << std::endl;
    std::cout << "=== All Concurrent Tests Passed ===" << std::endl;
    return 0;
}
