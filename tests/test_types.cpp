// SPDX-License-Identifier: MIT
/**
 * @file test_types.cpp
 * @brief Type safety and different data types tests
 */

#include <mpmc_queue.hpp>
#include <iostream>
#include <string>

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

struct CustomType {
    int id;
    double value;
    
    CustomType() : id(0), value(0.0) {}
    CustomType(int i, double v) : id(i), value(v) {}
    
    bool operator==(const CustomType& other) const {
        return id == other.id && value == other.value;
    }
};

int main() {
    std::cout << "=== Running Type Safety Tests ===" << std::endl << std::endl;

    // Test 1: Integer types
    {
        mpmc::MPMCQueue<int, 8> queue;
        
        [[maybe_unused]] bool e1 = queue.try_enqueue(42);
        [[maybe_unused]] bool e2 = queue.try_enqueue(-10);
        [[maybe_unused]] bool e3 = queue.try_enqueue(0);
        
        int val;
        [[maybe_unused]] bool d1 = queue.try_dequeue(val);
        TEST_ASSERT(val == 42, "Integer value 1 should match");
        [[maybe_unused]] bool d2 = queue.try_dequeue(val);
        TEST_ASSERT(val == -10, "Integer value 2 should match");
        [[maybe_unused]] bool d3 = queue.try_dequeue(val);
        TEST_ASSERT(val == 0, "Integer value 3 should match");
        
        TEST_PASS("Integer types");
    }

    // Test 2: Floating point types
    {
        mpmc::MPMCQueue<double, 8> queue;
        
        [[maybe_unused]] bool e1 = queue.try_enqueue(3.14159);
        [[maybe_unused]] bool e2 = queue.try_enqueue(-2.71828);
        [[maybe_unused]] bool e3 = queue.try_enqueue(0.0);
        
        double val;
        [[maybe_unused]] bool d1 = queue.try_dequeue(val);
        TEST_ASSERT(val == 3.14159, "Double value 1 should match");
        [[maybe_unused]] bool d2 = queue.try_dequeue(val);
        TEST_ASSERT(val == -2.71828, "Double value 2 should match");
        [[maybe_unused]] bool d3 = queue.try_dequeue(val);
        TEST_ASSERT(val == 0.0, "Double value 3 should match");
        
        TEST_PASS("Floating point types");
    }

    // Test 3: String types
    {
        mpmc::MPMCQueue<std::string, 8> queue;
        
        [[maybe_unused]] bool e1 = queue.try_enqueue("Hello");
        [[maybe_unused]] bool e2 = queue.try_enqueue("World");
        [[maybe_unused]] bool e3 = queue.try_enqueue("C++26");
        
        std::string val;
        [[maybe_unused]] bool d1 = queue.try_dequeue(val);
        TEST_ASSERT(val == "Hello", "String value 1 should match");
        [[maybe_unused]] bool d2 = queue.try_dequeue(val);
        TEST_ASSERT(val == "World", "String value 2 should match");
        [[maybe_unused]] bool d3 = queue.try_dequeue(val);
        TEST_ASSERT(val == "C++26", "String value 3 should match");
        
        TEST_PASS("String types");
    }

    // Test 4: Custom struct types
    {
        mpmc::MPMCQueue<CustomType, 8> queue;
        
        CustomType obj1(1, 1.5);
        CustomType obj2(2, 2.5);
        CustomType obj3(3, 3.5);
        
        [[maybe_unused]] bool e1 = queue.try_enqueue(obj1);
        [[maybe_unused]] bool e2 = queue.try_enqueue(obj2);
        [[maybe_unused]] bool e3 = queue.try_enqueue(obj3);
        
        CustomType val;
        [[maybe_unused]] bool d1 = queue.try_dequeue(val);
        TEST_ASSERT(val == obj1, "Custom type value 1 should match");
        [[maybe_unused]] bool d2 = queue.try_dequeue(val);
        TEST_ASSERT(val == obj2, "Custom type value 2 should match");
        [[maybe_unused]] bool d3 = queue.try_dequeue(val);
        TEST_ASSERT(val == obj3, "Custom type value 3 should match");
        
        TEST_PASS("Custom struct types");
    }

    // Test 5: Pointer types
    {
        mpmc::MPMCQueue<int*, 8> queue;
        
        int a = 10, b = 20, c = 30;
        [[maybe_unused]] bool e1 = queue.try_enqueue(&a);
        [[maybe_unused]] bool e2 = queue.try_enqueue(&b);
        [[maybe_unused]] bool e3 = queue.try_enqueue(&c);
        
        int* val;
        [[maybe_unused]] bool d1 = queue.try_dequeue(val);
        TEST_ASSERT(*val == 10, "Pointer value 1 should match");
        [[maybe_unused]] bool d2 = queue.try_dequeue(val);
        TEST_ASSERT(*val == 20, "Pointer value 2 should match");
        [[maybe_unused]] bool d3 = queue.try_dequeue(val);
        TEST_ASSERT(*val == 30, "Pointer value 3 should match");
        
        TEST_PASS("Pointer types");
    }

    // Test 6: Size types (size_t, uint64_t)
    {
        mpmc::MPMCQueue<size_t, 8> queue;
        
        [[maybe_unused]] bool e1 = queue.try_enqueue(0ULL);
        [[maybe_unused]] bool e2 = queue.try_enqueue(18446744073709551615ULL); // max uint64_t
        [[maybe_unused]] bool e3 = queue.try_enqueue(12345ULL);
        
        size_t val;
        [[maybe_unused]] bool d1 = queue.try_dequeue(val);
        TEST_ASSERT(val == 0ULL, "Size_t value 1 should match");
        [[maybe_unused]] bool d2 = queue.try_dequeue(val);
        TEST_ASSERT(val == 18446744073709551615ULL, "Size_t value 2 should match");
        [[maybe_unused]] bool d3 = queue.try_dequeue(val);
        TEST_ASSERT(val == 12345ULL, "Size_t value 3 should match");
        
        TEST_PASS("Size types");
    }

    std::cout << std::endl;
    std::cout << "=== All Type Safety Tests Passed ===" << std::endl;
    return 0;
}
