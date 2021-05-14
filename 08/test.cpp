#include <iostream>
#include <future>

#include "ThreadPool.h"
#include "test_runner.h"

namespace {

void TestEmpty() {
    try {
        ThreadPool pool(0);
        ASSERT(false);
    } catch (const std::runtime_error&) {
        ASSERT(true);
    }
}


int bar(int i) {
    return i;
}

void foo(std::string& str) {
    str += str;
}

void TestValid() {
    ThreadPool pool(8);
    auto task0 = pool.exec(bar, 5);
    ASSERT(task0.get() == 5);

    std::string str = "foo";
    auto task1 = pool.exec(foo, std::ref(str));
    task1.get();
    ASSERT_EQUAL(str, "foofoo")

    auto task2 = pool.exec([] { return 1; });
    ASSERT(task2.get() == 1);
}

}  // namespace

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestEmpty);
    RUN_TEST(tr, TestValid);
}
