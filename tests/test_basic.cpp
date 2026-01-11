// SPDX-License-Identifier: MIT
/**
 * @file test_basic.cpp
 * @brief Basic functionality tests for MPMCQueue
 */

#include <mpmc_queue.hpp>
#include <iostream>
#include <cassert>

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
        mpmc::MPMCQueue<int, 8> queue;
        TEST_ASSERT(queue.capacity() == 8, "Queue capacity should be 8");
        TEST_ASSERT(queue.empty_approx(), "New queue should be empty");
        TEST_ASSERT(queue.size_approx() == 0, "New queue size should be 0");
        TEST_PASS("Queue creation and capacity");
    }

    // Test 2: Basic enqueue/dequeue
    {
        mpmc::MPMCQueue<int, 8> queue;
        int value;
        
        TEST_ASSERT(queue.try_enqueue(42), "Enqueue should succeed");
        TEST_ASSERT(!queue.empty_approx(), "Queue should not be empty after enqueue");
        TEST_ASSERT(queue.size_approx() > 0, "Queue size should be > 0");
        
        TEST_ASSERT(queue.try_dequeue(value), "Dequeue should succeed");
        TEST_ASSERT(value == 42, "Dequeued value should be 42");
        TEST_ASSERT(queue.empty_approx(), "Queue should be empty after dequeue");
        TEST_PASS("Basic enqueue/dequeue");
    }

    // Test 3: FIFO order
    {
        mpmc::MPMCQueue<int, 8> queue;
        int values[] = {1, 2, 3, 4, 5};
        
        for (int val : values) {
            TEST_ASSERT(queue.try_enqueue(val), "Enqueue should succeed");
        }
        
        for (int expected : values) {
            int actual;
            TEST_ASSERT(queue.try_dequeue(actual), "Dequeue should succeed");
            TEST_ASSERT(actual == expected, "Values should be dequeued in FIFO order");
        }
        TEST_PASS("FIFO order preservation");
    }

    // Test 4: Queue full condition
    {
        mpmc::MPMCQueue<int, 4> queue;
        
        // Fill the queue (capacity is 4)
        TEST_ASSERT(queue.try_enqueue(1), "Enqueue 1 should succeed");
        TEST_ASSERT(queue.try_enqueue(2), "Enqueue 2 should succeed");
        TEST_ASSERT(queue.try_enqueue(3), "Enqueue 3 should succeed");
        TEST_ASSERT(queue.try_enqueue(4), "Enqueue 4 should succeed");
        
        // Queue should be full now
        TEST_ASSERT(!queue.try_enqueue(5), "Enqueue should fail when queue is full");
        TEST_PASS("Queue full condition");
    }

    // Test 5: Queue empty condition
    {
        mpmc::MPMCQueue<int, 4> queue;
        int value;
        
        TEST_ASSERT(!queue.try_dequeue(value), "Dequeue should fail on empty queue");
        
        [[maybe_unused]] bool enqueued = queue.try_enqueue(42);
        [[maybe_unused]] bool dequeued = queue.try_dequeue(value);
        
        TEST_ASSERT(!queue.try_dequeue(value), "Dequeue should fail after emptying queue");
        TEST_PASS("Queue empty condition");
    }

    // Test 6: Multiple enqueue/dequeue cycles
    {
        mpmc::MPMCQueue<int, 8> queue;
        
        for (int cycle = 0; cycle < 5; ++cycle) {
            for (int i = 0; i < 5; ++i) {
                TEST_ASSERT(queue.try_enqueue(cycle * 10 + i), "Enqueue should succeed");
            }
            
            for (int i = 0; i < 5; ++i) {
                int value;
                TEST_ASSERT(queue.try_dequeue(value), "Dequeue should succeed");
                TEST_ASSERT(value == cycle * 10 + i, "Value should match");
            }
        }
        TEST_PASS("Multiple enqueue/dequeue cycles");
    }

    // Test 7: Move semantics
    {
        mpmc::MPMCQueue<int, 8> queue;
        int value = 99;
        
        TEST_ASSERT(queue.try_enqueue(std::move(value)), "Move enqueue should succeed");
        
        int result;
        TEST_ASSERT(queue.try_dequeue(result), "Dequeue should succeed");
        TEST_ASSERT(result == 99, "Moved value should be correct");
        TEST_PASS("Move semantics");
    }

    // Test 8: Wraparound in ring buffer
    {
        mpmc::MPMCQueue<int, 4> queue;
        
        // Fill and empty multiple times to test wraparound
        for (int cycle = 0; cycle < 10; ++cycle) {
            for (int i = 0; i < 4; ++i) {
                TEST_ASSERT(queue.try_enqueue(cycle * 4 + i), "Enqueue should succeed");
            }
            
            for (int i = 0; i < 4; ++i) {
                int value;
                TEST_ASSERT(queue.try_dequeue(value), "Dequeue should succeed");
                TEST_ASSERT(value == cycle * 4 + i, "Value should match after wraparound");
            }
        }
        TEST_PASS("Ring buffer wraparound");
    }

    std::cout << std::endl;
    std::cout << "=== All Basic Tests Passed ===" << std::endl;
    return 0;
}
