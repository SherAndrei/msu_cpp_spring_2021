#include <string>

#include "allocator.h"
#include "test_runner.h"

void TestMakeAlloc();
void TestAlloc();
void TestReset();
void TestUsage();

void TestMakeAlloc() {
    {
        Allocator a;
        ASSERT(a.max_size() == 0u);

        a.makeAllocator(100);
        ASSERT(a.max_size() == 100u);

        a.makeAllocator(200);
        ASSERT(a.max_size() == 200u);
    }
    {
        Allocator a(100);
        ASSERT(a.max_size() == 100u);
    }
}

void TestAlloc() {
    {
        Allocator a(50);

        void* ptr = a.alloc(40);
        ASSERT(ptr != nullptr);

        ptr = a.alloc(40);
        ASSERT(ptr == nullptr);
    }
    {
        Allocator a;
        void* ptr = a.alloc(100);
        ASSERT(ptr == nullptr);

        a.makeAllocator(100);
        ptr = a.alloc(100);
        ASSERT(ptr != nullptr);
    }
}

void TestReset() {
    {
        Allocator a(20);
        char* ptr = a.alloc(20);
        ASSERT(ptr != nullptr);

        ptr = a.alloc(10);
        ASSERT(ptr == nullptr);

        a.reset();

        ptr = a.alloc(10);
        ASSERT(ptr != nullptr);
    }
    {
        Allocator a;
        a.reset();
        ASSERT(a.max_size() == 0u && a.alloc(10) == nullptr);
    }
}

void TestUsage() {
    Allocator a(7);
    char* sph = a.alloc(3);
    sph[0] = 's';
    sph[1] = 'p';
    sph[2] = 'h';

    char *er = a.alloc(2);
    er[0] = 'e';
    er[1] = 'r';

    char* e = a.alloc(2);
    e[0] = 'e';
    e[1] = '\0';

    char* more = a.alloc(10);
    ASSERT(more == nullptr);

    a.reset();

    ASSERT(std::string(a.alloc(7)) == std::string("sphere"));
}


int main() {
    TestRunner tr;
    RUN_TEST(tr, TestMakeAlloc);
    RUN_TEST(tr, TestAlloc);
    RUN_TEST(tr, TestReset);
    RUN_TEST(tr, TestUsage);
}
