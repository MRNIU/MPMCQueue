// SPDX-License-Identifier: MIT
/**
 * @file test_basic.cpp
 * @brief Basic functionality tests for MPMCQueue
 */

#include <MPMCQueue.hpp>
#include <iostream>
#include <cassert>
#include <utility>

using namespace mpmc_queue;

// Simple test framework macros
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
    std::cout << "=== Running Basic Functionality Tests ===" << std::endl << std::endl;

    // Test 1: Queue creation and capacity
    {
        MPMCQueue<int, 8> queue;
        TEST_ASSERT(queue.max_size() == 8, "Queue capacity should be 8");
        TEST_ASSERT(queue.empty(), "New queue should be empty");
        TEST_ASSERT(queue.size() == 0, "New queue size should be 0");
        TEST_PASS("Queue creation and capacity");
    }

    // Test 2: Basic enqueue/dequeue
    {
        MPMCQueue<int, 8> queue;
        int value;
        
        TEST_ASSERT(queue.push(42), "Enqueue should succeed");
        TEST_ASSERT(!queue.empty(), "Queue should not be empty after enqueue");
        TEST_ASSERT(queue.size() > 0, "Queue size should be > 0");
        
        TEST_ASSERT(queue.pop(value), "Dequeue should succeed");
        TEST_ASSERT(value == 42, "Dequeued value should be 42");
        TEST_ASSERT(queue.empty(), "Queue should be empty after dequeue");
        TEST_PASS("Basic enqueue/dequeue");
    }

    // Test 3: FIFO order
    {
        MPMCQueue<int, 8> queue;
        int values[] = {1, 2, 3, 4, 5};
        
        for (int val : values) {
            TEST_ASSERT(queue.push(val), "Enqueue should succeed");
        }
        
        for (int expected : values) {
            int actual;
            TEST_ASSERT(queue.pop(actual), "Dequeue should succeed");
            TEST_ASSERT(actual == expected, "Values should be dequeued in FIFO order");
        }
        TEST_PASS("FIFO order preservation");
    }

    // Test 4: Queue full condition
    {
        MPMCQueue<int, 4> queue;
        
        // Fill the queue (capacity is 4)
        TEST_ASSERT(queue.push(1), "Enqueue 1 should succeed");
        TEST_ASSERT(queue.push(2), "Enqueue 2 should succeed");
        TEST_ASSERT(queue.push(3), "Enqueue 3 should succeed");
        TEST_ASSERT(queue.push(4), "Enqueue 4 should succeed");
        
        // Queue should be full now
        TEST_ASSERT(!queue.push(5), "Enqueue should fail when queue is full");
        TEST_PASS("Queue full condition");
    }

    // Test 5: Queue empty condition
    {
        MPMCQueue<int, 4> queue;
        int value;
        
        TEST_ASSERT(!queue.pop(value), "Dequeue should fail on empty queue");
        
        [[maybe_unused]] bool enqueued = queue.push(42);
        [[maybe_unused]] bool dequeued = queue.pop(value);
        
        TEST_ASSERT(!queue.pop(value), "Dequeue should fail after emptying queue");
        TEST_PASS("Queue empty condition");
    }

    // Test 6: Multiple enqueue/dequeue cycles
    {
        MPMCQueue<int, 8> queue;
        for (int i = 0; i < 100; ++i) {
             TEST_ASSERT(queue.push(i), "Enqueue should succeed");
             int val;
             TEST_ASSERT(queue.pop(val), "Dequeue should succeed");
             TEST_ASSERT(val == i, "Value mismatch");
        }
        TEST_PASS("Multiple enqueue/dequeue cycles");
    }

    std::cout << "All basic tests passed!" << std::endl;
    return 0;
}
