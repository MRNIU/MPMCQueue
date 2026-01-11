// SPDX-License-Identifier: MIT
/**
 * @file basic_example.cpp
 * @brief Basic usage example of MPMCQueue
 */

#include <MPMCQueue.hpp>
#include <iostream>

using namespace mpmc_queue;

int main() {
    // Create a queue with capacity of 8 elements
    MPMCQueue<int, 8> queue;

    std::cout << "=== Basic MPMCQueue Example ===" << std::endl;
    std::cout << "Queue capacity: " << queue.max_size() << std::endl;
    std::cout << "Initial size: " << queue.size() << std::endl;
    std::cout << std::endl;

    // Enqueue some items
    std::cout << "Enqueuing items: 10, 20, 30, 40, 50" << std::endl;
    [[maybe_unused]] bool r1 = queue.push(10);
    [[maybe_unused]] bool r2 = queue.push(20);
    [[maybe_unused]] bool r3 = queue.push(30);
    [[maybe_unused]] bool r4 = queue.push(40);
    [[maybe_unused]] bool r5 = queue.push(50);
    
    std::cout << "Size after enqueue: " << queue.size() << std::endl;
    std::cout << "Is empty? " << (queue.empty() ? "Yes" : "No") << std::endl;
    std::cout << std::endl;

    // Dequeue and print items
    std::cout << "Dequeuing items:" << std::endl;
    int value;
    while (queue.pop(value)) {
        std::cout << "  Dequeued: " << value << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Size after dequeue: " << queue.size() << std::endl;
    std::cout << "Is empty? " << (queue.empty() ? "Yes" : "No") << std::endl;
    
    // Test overflow
    std::cout << std::endl;
    std::cout << "Testing queue overflow (capacity = " << queue.max_size() << "):" << std::endl;
    int successful_enqueues = 0;
    for (int i = 0; i < 20; ++i) {
        if (queue.push(i * 10)) {
            ++successful_enqueues;
        }
    }
    std::cout << "Successfully enqueued " << successful_enqueues << " items out of 20 attempts" << std::endl;
    std::cout << "Queue size: " << queue.size() << std::endl;
    
    // Test underflow
    std::cout << std::endl;
    std::cout << "Dequeuing all items:" << std::endl;
    int dequeue_count = 0;
    while (queue.pop(value)) {
        ++dequeue_count;
    }
    std::cout << "Dequeued " << dequeue_count << " items" << std::endl;
    
    // Try to dequeue from empty queue
    std::cout << "Attempting to dequeue from empty queue: " 
              << (queue.pop(value) ? "Success" : "Failed (as expected)") 
              << std::endl;

    std::cout << std::endl;
    std::cout << "=== Example completed successfully ===" << std::endl;
    
    return 0;
}
