#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <limits>

#include "test_runner.h"
#include "bigint.h"
#include "biginterr.h"

void TestIOstream();
void TestBigIntComparison();
void TestStringConstructor();
void TestIntegerConctructor();

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

void TestBigIntComparison() {
    ASSERT_EQUAL(BigInt("42"), BigInt("42"));
    ASSERT_EQUAL(BigInt("0"), BigInt("-0"));
    ASSERT_EQUAL(BigInt("00000000000000000000000000000000000000000000000000000000000001"), BigInt("1"));
    ASSERT_EQUAL(BigInt("0123456789123456789"), BigInt("123456789123456789"));
    ASSERT_EQUAL(BigInt("00042"), BigInt(42));

    ASSERT(BigInt("98936913561937591991369175") != BigInt("-98936913561937591991369175"));
    ASSERT(BigInt("111111111111111111") != BigInt("111111"));

    ASSERT(BigInt(2)  < BigInt(3));
    ASSERT(BigInt(-1) < BigInt(1));
    ASSERT(BigInt(-3) < BigInt(-2));
    ASSERT(BigInt("-11111111") < BigInt(111));
    ASSERT(BigInt("123123123123123123") < BigInt("456456456456456456"));
    ASSERT(BigInt(std::numeric_limits<long long>::max()) < BigInt("11111111111111111111111111111111"));
    ASSERT(BigInt("-11111111111111111111111111111111") < BigInt(std::numeric_limits<long long>::min()));
    ASSERT(BigInt(42) < BigInt(43));
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestIOstream);
    RUN_TEST(tr, TestStringConstructor);
    RUN_TEST(tr, TestIntegerConctructor);
    RUN_TEST(tr, TestBigIntComparison);
}
