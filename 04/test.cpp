#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <limits>

#include "test_runner.h"
#include "simple_vector.h"
#include "bigint.h"
#include "biginterr.h"

void TestConstruction();
void TestPushBack();
void TestNoCopy();
void TestComparison();

void TestIOstream();
void TestStringConstructor();
void TestIntegerConctructor();

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

namespace {

constexpr std::array<std::string_view, 13> testStrArr = {
        "0", "-1", "1",
        "-32768", "32767", "65535",
        "-9223372036854775808", "9223372036854775807", "18446744073709551615",
        "92233720368547758081844674407370955161518446744073709551615",
        "-184467440737095516151844674407370955161518446744073709551615",
        "10000000000000000000000000000000000000",
        "-10000000000000000000000000000000000000"
    };

}  // namespace

void TestIOstream() {
    for (auto&& str : testStrArr) {
        std::string strn = std::string(str);
        std::istringstream is(strn);
        BigInt bint;
        is >> bint;
        ASSERT_EQUAL(bint.to_string(), str);
        std::ostringstream os;
        os << bint;
        ASSERT_EQUAL(bint.to_string(), os.str());
    }
}

void TestStringConstructor() {
    for (auto&& str : testStrArr) {
        std::ostringstream os;
        BigInt bint(str);
        os << bint;
        ASSERT_EQUAL(bint.to_string(), os.str());
    }

    ASSERT_EQUAL(BigInt("00000000000000000000000000000000000000000000000").to_string(), "0");
    ASSERT_EQUAL(BigInt("00000000000000000000000000000000000000000000001").to_string(), "1");
    ASSERT_EQUAL(BigInt("-0000000000000000000000000000000000000000000001").to_string(), "-1");
}

void TestIntegerConctructor() {
    auto do_assert = [](Integral auto number) {
        ASSERT_EQUAL(BigInt(number).to_string(), std::to_string(number));
    };

    for (int number : {0, -1, 1,
                       -1'000'000'000,
                        1'000'000'000,
                       std::numeric_limits<int>::min(),
                       std::numeric_limits<int>::max()})
        do_assert(number);

    do_assert(4294967295u);

    for (long long number : {1'000'000'000'000'000'000ll,
                            -1'000'000'000'000'000'000ll,
                            std::numeric_limits<long long>::min(),
                            std::numeric_limits<long long>::max()})
        do_assert(number);

    do_assert(std::numeric_limits<unsigned long long>::max());
}


int main() {
    TestRunner tr;
    {
        std::cerr << "SIMPLE VECTOR TESTS:\n";
        RUN_TEST(tr, TestConstruction);
        RUN_TEST(tr, TestPushBack);
        RUN_TEST(tr, TestNoCopy);
        RUN_TEST(tr, TestComparison);
        std::cerr << "====================\n";
    }
    {
        std::cerr << "BIGINT TESTS:\n";
        RUN_TEST(tr, TestIOstream);
        RUN_TEST(tr, TestStringConstructor);
        RUN_TEST(tr, TestIntegerConctructor);
        std::cerr << "====================\n";
    }
}
