// SPDX-License-Identifier: MIT
/**
 * @file test_concurrent.cpp
 * @brief Concurrent usage tests for MPMCQueue
 */

#include <MPMCQueue.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <numeric>
#include <algorithm>
#include <cassert>

using namespace mpmc_queue;

#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "FAIL: " << message << std::endl; \
            std::exit(1); \
        } \
    } while (0)

#define TEST_PASS(message) \
    std::cout << "PASS: " << message << std::endl

constexpr int OPS_PER_THREAD = 10000;
constexpr int NUM_THREADS = 4;
constexpr int Q_SIZE = 4096;

void test_concurrent_push_pop() {
    std::cout << "Running concurrent push/pop test..." << std::endl;
    
    MPMCQueue<int, Q_SIZE> queue;
    std::atomic<int> sum_produced{0};
    std::atomic<int> sum_consumed{0};
    
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;
    
    // Producers
    for (int i = 0; i < NUM_THREADS; ++i) {
        producers.emplace_back([&, i]() {
            int local_sum = 0;
            for (int k = 0; k < OPS_PER_THREAD; ++k) {
                int val = i * OPS_PER_THREAD + k; // Unique value
                while (!queue.push(val)) {
                    std::this_thread::yield();
                }
                local_sum += val;
            }
            sum_produced += local_sum;
        });
    }
    
    // Consumers
    for (int i = 0; i < NUM_THREADS; ++i) {
        consumers.emplace_back([&]() {
            int local_sum = 0;
            for (int k = 0; k < OPS_PER_THREAD; ++k) {
                int val;
                while (!queue.pop(val)) {
                    std::this_thread::yield();
                }
                local_sum += val;
            }
            sum_consumed += local_sum;
        });
    }
    
    for (auto& t : producers) t.join();
    for (auto& t : consumers) t.join();
    
    TEST_ASSERT(queue.empty(), "Queue should be empty after all consumptions");
    TEST_ASSERT(sum_produced == sum_consumed, "Produced sum should match consumed sum");
    
    TEST_PASS("Concurrent push/pop");
}

int main() {
    test_concurrent_push_pop();
    std::cout << "All concurrent tests passed!" << std::endl;
    return 0;
}
