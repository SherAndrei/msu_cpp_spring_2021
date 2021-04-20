#include "test_runner.h"
#include "simple_vector.h"


void TestConstruction();
void TestPushBack();
void TestNoCopy();
void TestComparison();

void TestConstruction() {
    SimpleVector<int> empty;
    ASSERT_EQUAL(empty.size(), 0u);
    ASSERT_EQUAL(empty.capacity(), 0u);
    ASSERT(empty.begin() == empty.end());

    SimpleVector<std::string> five_strings(5);
    ASSERT_EQUAL(five_strings.size(), 5u);
    ASSERT(five_strings.size() <= five_strings.capacity());
    for (auto& item : five_strings) {
        ASSERT(item.empty());
    }
    five_strings[2] = "Hello";
    ASSERT_EQUAL(five_strings[2], "Hello");
}

void TestPushBack() {
    SimpleVector<int> v;
    for (int i = 10; i >= 1; --i) {
        v.push_back(i);
        ASSERT(v.size() <= v.capacity());
    }
    std::sort(v.begin(), v.end());

    const std::vector<int> expected = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    ASSERT(std::equal(v.begin(), v.end(), begin(expected)));
}

class StringNonCopyable : public std::string {
 public:
    using std::string::string;
    explicit StringNonCopyable(std::string&& other) : std::string(std::move(other)) {}
    StringNonCopyable(const StringNonCopyable&) = delete;
    StringNonCopyable(StringNonCopyable&&) = default;
    StringNonCopyable& operator=(const StringNonCopyable&) = delete;
    StringNonCopyable& operator=(StringNonCopyable&&) = default;
};

void TestNoCopy() {
    SimpleVector<StringNonCopyable> strings;
    static const int size = 10;
    for (int i = 0; i < size; ++i) {
        strings.push_back(StringNonCopyable(std::to_string(i)));
    }
    for (int i = 0; i < size; ++i) {
        ASSERT_EQUAL(strings[i], std::to_string(i));
    }
}

void TestComparison() {
    auto to_svector = [](const std::vector<int>& v) {
        SimpleVector<int> svec(v.size());
        std::for_each(v.begin(), v.end(), [&] (int num) {
            svec.push_back(num);
        });
        return svec;
    };

    ASSERT(to_svector({}) == to_svector({}));
    ASSERT(to_svector({0, 1, 2}) == to_svector({0, 1, 2}));
    ASSERT(to_svector({0, 1, 2}) <= to_svector({0, 1, 2}));
    ASSERT(to_svector({0, 1, 2}) >= to_svector({0, 1, 2}));
    ASSERT(to_svector({2, 1}) > to_svector({0, 1, 2}));
    ASSERT(to_svector({0, 1, 2}) < to_svector({2, 1}));
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestConstruction);
    RUN_TEST(tr, TestPushBack);
    RUN_TEST(tr, TestNoCopy);
    RUN_TEST(tr, TestComparison);
}
