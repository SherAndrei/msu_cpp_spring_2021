#include <string_view>

#include "test_runner.h"
#include "Vector.h"


void TestConstruction();
void TestCopyConstruction();
void TestMoveConstruction();
void TestInitializerList();

void TestPushBack();
void TestEmplaceBack();

void TestReserve();
void TestResize();

void TestComparison();

struct Counter {
    static inline size_t ctor_calls = 0;
    static inline size_t dtor_calls = 0;
    static inline size_t copy_ctor_calls = 0;
    static inline size_t move_ctor_calls = 0;
    static inline size_t copy_op_calls = 0;
    static inline size_t move_op_calls = 0;

     Counter() { ctor_calls++; }
    ~Counter() { dtor_calls++; }
    Counter(const Counter&)     { copy_ctor_calls++; }
    Counter(Counter&&) noexcept { move_ctor_calls++; }
    Counter& operator=(const Counter&)     { copy_op_calls++; return *this; }
    Counter& operator=(Counter&&) noexcept { move_op_calls++; return *this; }

    static void clear() {
        ctor_calls      = dtor_calls      = 0;
        copy_ctor_calls = move_ctor_calls = 0;
        copy_op_calls   = move_op_calls   = 0;
    }
};

namespace {

constexpr
std::string_view long_sv = "The quick brown fox jumps over the lazy dog";

}

void TestConstruction() {
    {
        Vector<int> empty;
        ASSERT_EQUAL(empty.size(), 0u);
        ASSERT_EQUAL(empty.capacity(), 0u);
        ASSERT(empty.begin() == empty.end());
    }
    {
        ASSERT_EQUAL(Counter::ctor_calls, 0ul);
        ASSERT_EQUAL(Counter::dtor_calls, 0ul);
        {
            Vector<Counter> cvec(10);
        }
        ASSERT_EQUAL(Counter::ctor_calls, 10ul);
        ASSERT_EQUAL(Counter::dtor_calls, 10ul);
        Counter::ctor_calls = Counter::dtor_calls = 0;
    }
    {
        Vector<char> chars(24, 'a');
        for (auto&& ch : chars)
            ASSERT_EQUAL(ch, 'a')
        ASSERT_EQUAL(chars.size(), 24u);
    }
    {
        // no sso optimization
        Vector<std::string> five_strings(5, std::string(long_sv));
        ASSERT_EQUAL(five_strings.size(), 5u);
        ASSERT(five_strings.size() <= five_strings.capacity());
        for (auto&& item : five_strings) {
            ASSERT(item == long_sv);
        }
    }
}

void TestInitializerList() {
    {
        Vector<int> ints({0, 1, 2});
        ASSERT_EQUAL(ints.size(), 3u);
        for (int i = 0; i < 3; i++)
            ASSERT_EQUAL(ints[i], i);
        ints = {2, 1, 0};
        for (int i = 0; i < 3; i++)
            ASSERT_EQUAL(ints[i], 2 - i);
    }
    {
        Vector<int> ints{};
        ASSERT_EQUAL(ints.size(), 0ul);
        ASSERT_EQUAL(ints.capacity(), 0ul);
    }
    {
        Vector<Vector<int>> matrix = {
            {0, 1, 2},
            {3, 4, 5},
            {6, 7, 8}
        };
        int i = 0;
        for (auto&& row : matrix) {
            ASSERT_EQUAL(row.size(), 3ul);
            ASSERT(row == Vector<int>({i, i + 1, i + 2}));
            i += 3;
        }
    }
}

void TestCopyConstruction() {
    {
        Vector<int> empty({});
        Vector<int> full({1, 3, 5, 7, 9});
        ASSERT_EQUAL(full.size(), 5ul);
        full = empty;
        ASSERT_EQUAL(full.size(), 0ul);
    }
    {
        Vector<std::string> strings(5, std::string(long_sv));
        Vector<std::string> another_strings(strings);

        ASSERT_EQUAL(another_strings.size(), 5ul);
        for (auto&& str : another_strings)
            ASSERT_EQUAL(str, long_sv);

        Vector<std::string> hellos(3, "hello");
        another_strings = hellos;
        ASSERT_EQUAL(another_strings.size(), 3ul);
        for (auto&& str : another_strings)
            ASSERT_EQUAL(str, "hello");
    }
    {
        Vector<Counter> counters(10);
        ASSERT_EQUAL(Counter::ctor_calls, 10ul);
        Counter::ctor_calls = 0;

        Vector<Counter> another_counters(counters);
        ASSERT_EQUAL(Counter::ctor_calls, 0ul);
        ASSERT_EQUAL(Counter::copy_ctor_calls, 10ul);
        Counter::copy_ctor_calls = 0;

        Vector<Counter> yet_another_counters(100);
        ASSERT_EQUAL(Counter::ctor_calls, 100ul);

        yet_another_counters = another_counters;
        ASSERT_EQUAL(Counter::copy_ctor_calls, 10ul);
        ASSERT_EQUAL(Counter::dtor_calls, 100ul);
        Counter::dtor_calls = 0;
    }
    ASSERT_EQUAL(Counter::dtor_calls, 10ul + 10ul + 10ul);
    Counter::clear();
}

void TestMoveConstruction() {
    {
        Vector<int> ints = {1, 2, 4, 5};
        Vector<int> empty(std::move(ints));
        ASSERT(ints == Vector<int>{});
        ASSERT(empty == Vector<int>({1, 2, 4, 5}));
    }
    {
        Vector<std::string> strings(5, std::string(long_sv));
        Vector<std::string> another_strings(std::move(strings));

        ASSERT(strings == Vector<std::string>{});
        ASSERT_EQUAL(another_strings.size(), 5ul);
        for (auto&& str : another_strings)
            ASSERT_EQUAL(str, long_sv);

        Vector<std::string> hellos(3, "hello");
        another_strings = std::move(hellos);
        ASSERT_EQUAL(another_strings.size(), 3ul);
        for (auto&& str : another_strings)
            ASSERT_EQUAL(str, "hello");
        ASSERT(hellos == Vector<std::string>{});
    }
    {
        Vector<Counter> counters(10);
        ASSERT_EQUAL(Counter::ctor_calls, 10ul);
        Counter::ctor_calls = 0;

        Vector<Counter> another_counters(std::move(counters));
        ASSERT_EQUAL(Counter::copy_ctor_calls, 0ul);
        ASSERT_EQUAL(Counter::move_ctor_calls, 0ul);

        Vector<Counter> yet_another_counters(100);
        ASSERT_EQUAL(Counter::ctor_calls, 100ul);

        yet_another_counters = std::move(another_counters);
        ASSERT_EQUAL(Counter::dtor_calls, 100ul);
        ASSERT_EQUAL(Counter::copy_ctor_calls, 0ul);
        ASSERT_EQUAL(Counter::move_ctor_calls, 0ul);
        Counter::dtor_calls = 0;
    }
    ASSERT_EQUAL(Counter::dtor_calls, 10ul);
    Counter::clear();
}

void TestPushBack() {
    {
        Vector<int> v;
        for (int i = 10; i >= 1; --i) {
            v.push_back(i);
            ASSERT(v.size() <= v.capacity());
        }
        ASSERT_EQUAL(v.size(), 10ul);
        std::sort(v.begin(), v.end());

        const std::vector<int> expected = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        ASSERT(std::equal(v.begin(), v.end(), std::begin(expected)));
    }
    {
        Vector<std::string> v;
        for (int i = 0; i < 10; i++)
            v.push_back(std::string(long_sv));

        ASSERT_EQUAL(v.size(), 10ul);
        for (auto&& str : v)
            ASSERT_EQUAL(str, long_sv);
    }
}

void TestEmplaceBack() {
    {
        Vector<std::string> v;
        for (int i = 0; i < 10; i++)
            v.emplace_back(long_sv);

        ASSERT_EQUAL(v.size(), 10ul);
        for (auto&& str : v)
            ASSERT_EQUAL(str, long_sv);
    }
}

void TestReserve() {
    {
        Vector<int> ints({});
        ints.reserve(20);
        ASSERT_EQUAL(ints.size(), 0ul);
        ASSERT_EQUAL(ints.capacity(), 20ul);
    }
    {
        Vector<Counter> counters;
        for (int i = 0; i < 4; i++)
            counters.push_back(Counter());
        ASSERT_EQUAL(Counter::ctor_calls, 4ul);
        ASSERT_EQUAL(Counter::dtor_calls, 4ul);
        ASSERT_EQUAL(Counter::move_ctor_calls, 8ul);
        Counter::clear();

        Vector<Counter> smart_counters;
        smart_counters.reserve(4);
        for (int i = 0; i < 4; i++)
            smart_counters.push_back(Counter());
        ASSERT_EQUAL(Counter::ctor_calls, 4ul);
        ASSERT_EQUAL(Counter::dtor_calls, 4ul);
        ASSERT_EQUAL(Counter::move_ctor_calls, 4ul);
        Counter::clear();

        Vector<Counter> smarter_counters;
        smarter_counters.reserve(4);
        for (int i = 0; i < 4; i++)
            smarter_counters.emplace_back();
        ASSERT_EQUAL(Counter::ctor_calls, 4ul);
        ASSERT_EQUAL(Counter::dtor_calls, 0ul);
        ASSERT_EQUAL(Counter::move_ctor_calls, 0ul);
    }
    Counter::clear();
}

void TestResize() {
    {
        Vector<int> empty;
        empty.resize(10);
        ASSERT(empty.size() == 10ul);
        ASSERT(empty.capacity() == 10ul);
    }
    {
        Vector<Counter> counters(5);
        ASSERT_EQUAL(counters.size(), 5ul);
        ASSERT_EQUAL(counters.capacity(), 5ul);

        counters.resize(3);
        ASSERT_EQUAL(Counter::dtor_calls, 2ul);
        ASSERT_EQUAL(counters.size(), 3ul);
        ASSERT_EQUAL(counters.capacity(), 5ul);
        Counter::clear();

        counters.resize(4);
        ASSERT_EQUAL(Counter::ctor_calls, 1ul);
        ASSERT_EQUAL(Counter::dtor_calls, 0ul);
        ASSERT_EQUAL(counters.size(), 4ul);
        ASSERT_EQUAL(counters.capacity(), 5ul);
        Counter::clear();

        counters.resize(20);
        ASSERT_EQUAL(Counter::ctor_calls, 16ul);
        ASSERT_EQUAL(Counter::copy_op_calls, 0ul);
        ASSERT_EQUAL(counters.size(), 20ul);
        ASSERT_EQUAL(counters.capacity(), 20ul);
    }
    Counter::clear();
}

void TestComparison() {
    ASSERT(Vector<int>({})        == Vector<int>({}));
    ASSERT(Vector<int>({0, 1, 2}) == Vector<int>({0, 1, 2}));
    ASSERT(Vector<int>({0, 1, 2}) <= Vector<int>({0, 1, 2}));
    ASSERT(Vector<int>({0, 1, 2}) >= Vector<int>({0, 1, 2}));
    ASSERT(Vector<int>({2, 1})    >  Vector<int>({0, 1, 2}));
    ASSERT(Vector<int>({0, 1, 2}) <  Vector<int>({2, 1}));
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestConstruction);
    RUN_TEST(tr, TestCopyConstruction);
    RUN_TEST(tr, TestMoveConstruction);
    RUN_TEST(tr, TestInitializerList);

    RUN_TEST(tr, TestPushBack);
    RUN_TEST(tr, TestEmplaceBack);

    RUN_TEST(tr, TestReserve);
    RUN_TEST(tr, TestResize);

    RUN_TEST(tr, TestComparison);
}
