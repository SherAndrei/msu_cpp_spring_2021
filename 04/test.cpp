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
void TestUnary();
void TestSum();
void TestSub();
void TestMult();

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

using ll_lim  = std::numeric_limits<long long>;
using llu_lim = std::numeric_limits<long long unsigned>;

void TestBigIntComparison() {
    ASSERT_EQUAL(BigInt("42"), 42);
    ASSERT_EQUAL(42, BigInt("42"));
    ASSERT_EQUAL(BigInt("-42"), -42);
    ASSERT_EQUAL(-42, BigInt("-42"));
    ASSERT_EQUAL(BigInt("0"), BigInt("-0"));
    ASSERT_EQUAL(BigInt("00000000000000000000000000000000000000000000000000000000000001"), BigInt("1"));
    ASSERT_EQUAL(BigInt(llu_lim::max()),
                 BigInt(std::to_string(llu_lim::max())));

    ASSERT(BigInt("98936913561937591991369175") != BigInt("-98936913561937591991369175"));
    ASSERT(BigInt("100000000000000") < BigInt("1000000000000000"));

    ASSERT(BigInt(2)  < 3);
    ASSERT(BigInt(-1) < 1);
    ASSERT(BigInt(-3) < -2);
    ASSERT(BigInt(llu_lim::max()) < BigInt("11111111111111111111111111111111"));
    ASSERT(BigInt("-11111111111111111111111111111111") < ll_lim::min());
}

void TestUnary() {
    ASSERT_EQUAL(BigInt(-1), -BigInt(1));
    ASSERT_EQUAL(BigInt("-0"), -BigInt(0));
    ASSERT_EQUAL(BigInt(5), -BigInt(-5));
    ASSERT_EQUAL(BigInt(-5), -(-BigInt(-5)));
}

void TestSum() {
    ASSERT_EQUAL(BigInt(1) + BigInt(1), BigInt(2));
    ASSERT_EQUAL(BigInt(1) + BigInt(-1), BigInt(0));

    ASSERT_EQUAL(BigInt(ll_lim::max()) + BigInt(ll_lim::max()) + 1, BigInt(llu_lim::max()));
    ASSERT_EQUAL(BigInt(ll_lim::max()) + BigInt(ll_lim::min()), -1);

    ASSERT_EQUAL(BigInt("000000000000000000000000000000000000000001") +
                 BigInt("000000000000000000000000000000000000000001"), 2);
    ASSERT_EQUAL(BigInt("000000000000000000000000000000000000000001") +
                 BigInt("-000000000000000000000000000000000000000001"), 0);

    ASSERT_EQUAL(BigInt("100000000000000000000000000000000000000000") +
                 BigInt("100000000000000000000000000000000000000000"),
                 BigInt("200000000000000000000000000000000000000000"));
}

void TestSub() {
    ASSERT_EQUAL(BigInt(2) - BigInt(1), BigInt(1));
    ASSERT_EQUAL(BigInt(1) - (-BigInt(1)), BigInt(2));

    ASSERT_EQUAL(BigInt(llu_lim::max()) - BigInt(ll_lim::max()) - BigInt(ll_lim::max()), 1);
    ASSERT_EQUAL(-BigInt(ll_lim::max()) + BigInt(llu_lim::max()) - BigInt(ll_lim::max()), 1);
    ASSERT_EQUAL(-BigInt(ll_lim::max()) - BigInt(ll_lim::max()) + BigInt(llu_lim::max()), 1);
    ASSERT_EQUAL(BigInt(ll_lim::max()) + BigInt(1), -BigInt(ll_lim::min()));

    ASSERT_EQUAL(BigInt("000000000000000000000000000000000000000001") -
                 BigInt("000000000000000000000000000000000000000001"), 0);
    ASSERT_EQUAL(BigInt("000000000000000000000000000000000000000001") -
                 BigInt("-000000000000000000000000000000000000000001"), 2);

    ASSERT_EQUAL(BigInt("100000000000000000000000000000000000000000") -
                 BigInt("200000000000000000000000000000000000000000"),
                 BigInt("-100000000000000000000000000000000000000000"));
}

void TestMult() {
    ASSERT_EQUAL(BigInt(1) * 0, 0);
    ASSERT_EQUAL(BigInt(-1) * 0, BigInt(0));
    ASSERT_EQUAL(BigInt(1) * BigInt(1), 1);
    ASSERT_EQUAL(BigInt(2) * BigInt(2), BigInt(4));
    ASSERT_EQUAL(BigInt(-2) * BigInt(2), -4);
    ASSERT_EQUAL(BigInt(-2) * -2, 4);

    ASSERT_EQUAL(BigInt(ll_lim::max()) * 2 + 1, BigInt(llu_lim::max()));
    ASSERT_EQUAL(BigInt(llu_lim::max()) - BigInt(ll_lim::max()) * 2, 1);

    ASSERT_EQUAL(BigInt("10000000000000000000000000000") * 0, 0);
    ASSERT_EQUAL(BigInt("00000000000000000000000000001") * (-1), -1);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestIOstream);
    RUN_TEST(tr, TestStringConstructor);
    RUN_TEST(tr, TestIntegerConctructor);
    RUN_TEST(tr, TestBigIntComparison);
    RUN_TEST(tr, TestUnary);
    RUN_TEST(tr, TestSum);
    RUN_TEST(tr, TestSub);
    RUN_TEST(tr, TestMult);
}
