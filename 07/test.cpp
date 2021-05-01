#include "test_runner.h"
#include "Vector.h"


void TestConstruction();
void TestCopyConstruction();
void TestInitializerList();

void TestPushBack();
void TestNoCopy();
void TestComparison();

struct Counter {
    static inline int ctor_calls = 0;
    static inline int dtor_calls = 0;
     Counter() { ctor_calls++; }
    ~Counter() { dtor_calls++; }
};

void TestConstruction() {
    {
        Vector<int> empty;
        ASSERT_EQUAL(empty.size(), 0u);
        ASSERT_EQUAL(empty.capacity(), 0u);
        ASSERT(empty.begin() == empty.end());
    }
    {
        ASSERT_EQUAL(Counter::ctor_calls, 0);
        ASSERT_EQUAL(Counter::dtor_calls, 0);
        {
            Vector<Counter> cvec(10);
        }
        ASSERT_EQUAL(Counter::ctor_calls, 10);
        ASSERT_EQUAL(Counter::dtor_calls, 10);
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
        Vector<std::string> five_strings(5, std::string("The quick brown fox jumps over the lazy dog"));
        ASSERT_EQUAL(five_strings.size(), 5u);
        ASSERT(five_strings.size() <= five_strings.capacity());
        for (auto&& item : five_strings) {
            ASSERT(item == "The quick brown fox jumps over the lazy dog");
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
}

void TestCopyConstruction() {

}

// void TestPushBack() {
//     Vector<int> v;
//     for (int i = 10; i >= 1; --i) {
//         v.push_back(i);
//         ASSERT(v.size() <= v.capacity());
//     }
//     std::sort(v.begin(), v.end());

//     const std::vector<int> expected = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
//     ASSERT(std::equal(v.begin(), v.end(), begin(expected)));
// }

// class StringNonCopyable : public std::string {
//  public:
//     using std::string::string;
//     explicit StringNonCopyable(std::string&& other) : std::string(std::move(other)) {}
//     StringNonCopyable(const StringNonCopyable&) = delete;
//     StringNonCopyable(StringNonCopyable&&) = default;
//     StringNonCopyable& operator=(const StringNonCopyable&) = delete;
//     StringNonCopyable& operator=(StringNonCopyable&&) = default;
// };

// void TestNoCopy() {
//     Vector<StringNonCopyable> strings;
//     static const int size = 10;
//     for (int i = 0; i < size; ++i) {
//         strings.push_back(StringNonCopyable(std::to_string(i)));
//     }
//     for (int i = 0; i < size; ++i) {
//         ASSERT_EQUAL(strings[i], std::to_string(i));
//     }
// }

// void TestComparison() {
//     auto to_svector = [](const std::vector<int>& v) {
//         Vector<int> svec(v.size());
//         std::for_each(v.begin(), v.end(), [&] (int num) {
//             svec.push_back(num);
//         });
//         return svec;
//     };

//     ASSERT(to_svector({}) == to_svector({}));
//     ASSERT(to_svector({0, 1, 2}) == to_svector({0, 1, 2}));
//     ASSERT(to_svector({0, 1, 2}) <= to_svector({0, 1, 2}));
//     ASSERT(to_svector({0, 1, 2}) >= to_svector({0, 1, 2}));
//     ASSERT(to_svector({2, 1}) > to_svector({0, 1, 2}));
//     ASSERT(to_svector({0, 1, 2}) < to_svector({2, 1}));
// }

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestConstruction);
    RUN_TEST(tr, TestCopyConstruction);
    RUN_TEST(tr, TestInitializerList);
    // RUN_TEST(tr, TestPushBack);
    // RUN_TEST(tr, TestNoCopy);
    // RUN_TEST(tr, TestComparison);
}
