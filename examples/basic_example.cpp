// SPDX-License-Identifier: MIT
/**
 * @file basic_example.cpp
 * @brief Basic usage example of MPMCQueue
 */

#include <mpmc_queue.hpp>
#include <iostream>

int main() {
    // Create a queue with capacity of 8 elements
    mpmc::MPMCQueue<int, 8> queue;

    std::cout << "=== Basic MPMCQueue Example ===" << std::endl;
    std::cout << "Queue capacity: " << queue.capacity() << std::endl;
    std::cout << "Initial size: " << queue.size_approx() << std::endl;
    std::cout << std::endl;

    // Enqueue some items
    std::cout << "Enqueuing items: 10, 20, 30, 40, 50" << std::endl;
    queue.try_enqueue(10);
    queue.try_enqueue(20);
    queue.try_enqueue(30);
    queue.try_enqueue(40);
    queue.try_enqueue(50);
    
    std::cout << "Size after enqueue: " << queue.size_approx() << std::endl;
    std::cout << "Is empty? " << (queue.empty_approx() ? "Yes" : "No") << std::endl;
    std::cout << std::endl;

    // Dequeue and print items
    std::cout << "Dequeuing items:" << std::endl;
    int value;
    while (queue.try_dequeue(value)) {
        std::cout << "  Dequeued: " << value << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Size after dequeue: " << queue.size_approx() << std::endl;
    std::cout << "Is empty? " << (queue.empty_approx() ? "Yes" : "No") << std::endl;
    
    // Test overflow
    std::cout << std::endl;
    std::cout << "Testing queue overflow (capacity = " << queue.capacity() << "):" << std::endl;
    int successful_enqueues = 0;
    for (int i = 0; i < 20; ++i) {
        if (queue.try_enqueue(i * 10)) {
            ++successful_enqueues;
        }
    }
    std::cout << "Successfully enqueued " << successful_enqueues << " items out of 20 attempts" << std::endl;
    std::cout << "Queue size: " << queue.size_approx() << std::endl;
    
    // Test underflow
    std::cout << std::endl;
    std::cout << "Dequeuing all items:" << std::endl;
    int dequeue_count = 0;
    while (queue.try_dequeue(value)) {
        ++dequeue_count;
    }
    std::cout << "Dequeued " << dequeue_count << " items" << std::endl;
    
    // Try to dequeue from empty queue
    std::cout << "Attempting to dequeue from empty queue: " 
              << (queue.try_dequeue(value) ? "Success" : "Failed (as expected)") 
              << std::endl;

    std::cout << std::endl;
    std::cout << "=== Example completed successfully ===" << std::endl;
    
    return 0;
}
