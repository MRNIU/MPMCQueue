// SPDX-License-Identifier: MIT
/**
 * @file test_types.cpp
 * @brief Test MPMCQueue with different types (move-only, complex structs)
 */

#include <MPMCQueue.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

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

struct ComplexType {
    int id;
    std::string name;
    std::vector<double> data;
    
    bool operator==(const ComplexType& other) const {
        return id == other.id && name == other.name && data == other.data;
    }
};

void test_complex_types() {
    MPMCQueue<ComplexType, 8> queue;
    ComplexType t1{1, "test1", {1.1, 2.2}};
    ComplexType t2{2, "test2", {3.3, 4.4}};
    
    TEST_ASSERT(queue.push(t1), "Push complex type t1");
    TEST_ASSERT(queue.push(t2), "Push complex type t2");
    
    ComplexType out;
    TEST_ASSERT(queue.pop(out), "Pop t1");
    TEST_ASSERT(out == t1, "Popped t1 match");
    
    TEST_ASSERT(queue.pop(out), "Pop t2");
    TEST_ASSERT(out == t2, "Popped t2 match");
    
    TEST_PASS("Complex types supported");
}

void test_move_only() {
    MPMCQueue<std::unique_ptr<int>, 8> queue;
    
    auto ptr = std::make_unique<int>(123);
    // queue.push(ptr); // Should fail compilation or be deleted if we tried
    TEST_ASSERT(queue.push(std::move(ptr)), "Push move-only type");
    TEST_ASSERT(!ptr, "Original pointer should be null after move");
    
    std::unique_ptr<int> out;
    TEST_ASSERT(queue.pop(out), "Pop move-only type");
    TEST_ASSERT(out && *out == 123, "Popped value match");
    
    TEST_PASS("Move-only types supported");
}

int main() {
    test_complex_types();
    test_move_only();
    std::cout << "All type tests passed!" << std::endl;
    return 0;
}
