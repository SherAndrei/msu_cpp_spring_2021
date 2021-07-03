#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <limits>
#include <array>
#include <string_view>

#include "test_runner.h"
#include "bigint.h"
#include "biginterr.h"

namespace {

inline const std::array<std::string_view, 13> testStrArr = {
    "0", "-1", "1",
    "-32768", "32767", "65535",
    "-9223372036854775808", "9223372036854775807", "18446744073709551615",
    "92233720368547758081844674407370955161518446744073709551615",
    "-184467440737095516151844674407370955161518446744073709551615",
    "10000000000000000000000000000000000000",
    "-10000000000000000000000000000000000000"
};

void TestIOstream() {
    for (auto&& str : testStrArr) {
        std::string strn = std::string(str);
        std::istringstream is(strn);
        BigInt bint = 0;
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

    ASSERT_EQUAL(BigInt("00000000000000000000000000000000000000000000000"), 0);
    ASSERT_EQUAL(BigInt("00000000000000000000000000000000000000000000001"), 1);
    ASSERT_EQUAL(BigInt("-0000000000000000000000000000000000000000000001"), -1);
}

void TestIntegerConctructor() {
    auto do_assert = [](auto number) {
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

void TestCopy() {
    {
        BigInt lvalue(llu_lim::max());
        BigInt newvalue(lvalue);
        ASSERT_EQUAL(lvalue, llu_lim::max());
        ASSERT_EQUAL(newvalue, llu_lim::max());
    }
    {
        BigInt lvalue(llu_lim::max());
        BigInt newvalue(2);
        newvalue = lvalue;
        ASSERT_EQUAL(lvalue, llu_lim::max());
        ASSERT_EQUAL(newvalue, llu_lim::max());
    }
}

void TestMove() {
    {
        BigInt lvalue(llu_lim::max());
        BigInt newvalue(std::move(lvalue));
        ASSERT_EQUAL(lvalue.to_string(), "");
        ASSERT_EQUAL(newvalue, llu_lim::max());
    }
    {
        BigInt lvalue(llu_lim::max());
        BigInt newvalue(2);
        newvalue = std::move(lvalue);
        ASSERT_EQUAL(lvalue.to_string(), "");
        ASSERT_EQUAL(newvalue, llu_lim::max());
    }
}

void TestBigIntComparison() {
    using namespace std::literals::string_view_literals;

    const std::vector<std::pair<BigInt, BigInt>> l_g_vec = {
        {1, 3}, {"4"sv, 5}, {6, "7"sv}, {"8"sv, "9"sv},
        {-3, -2}, {-5, "-4"sv}, {"-7"sv, -6}, {"-9"sv, "-8"sv},
        {"100000000000000"sv, "1000000000000000"sv},
        {"-98936913561937591991369175"sv, "98936913561937591991369175"sv},
        {llu_lim::max(), "11111111111111111111111111111111"sv},
        {"-11111111111111111111111111111111"sv, ll_lim::min()}
    };

    for (auto&& [lhs, rhs] : l_g_vec) {
        ASSERT(lhs <  rhs);
        ASSERT(lhs <= rhs);
        ASSERT(rhs >  lhs);
        ASSERT(rhs >= lhs);
        ASSERT(lhs != rhs);
        ASSERT(rhs != lhs);
    }

    const std::vector<std::pair<BigInt, BigInt>> eq_vec = {
        {0, "-0"sv}, {"-0"sv, "-0"sv}, {0, 0},
        {"42"sv, 42}, {42, "42"sv},
        {"-42"sv, -42}, {-42, "-42"sv},
        {"000000000000000000000000000001"sv, "1"sv},
        {llu_lim::max(), std::string_view(std::to_string(llu_lim::max()))},
        {"111111111111111111111111111111"sv, "111111111111111111111111111111"sv},
        {"-111111111111111111111111111111"sv, "-111111111111111111111111111111"sv},
    };

    for (auto&& [lhs, rhs] : eq_vec) {
        ASSERT(lhs == rhs);
        ASSERT(lhs <= rhs);
        ASSERT(rhs <= lhs);
        ASSERT(lhs >= rhs);
        ASSERT(rhs >= lhs);
    }
}

void TestUnary() {
    ASSERT_EQUAL(BigInt(-1), -BigInt(1));
    ASSERT_EQUAL(BigInt("-0"), -BigInt(0));
    ASSERT_EQUAL(BigInt(5), -BigInt(-5));
    ASSERT_EQUAL(BigInt(-5), -(-BigInt(-5)));
}

void TestSum() {
    ASSERT_EQUAL(BigInt(0) + BigInt(0), 0);
    ASSERT_EQUAL(1 + BigInt(1), BigInt(2));
    ASSERT_EQUAL(BigInt(1) + BigInt(-1), BigInt(0));

    ASSERT_EQUAL(BigInt(ll_lim::max()) + BigInt(ll_lim::max()) + 1, BigInt(llu_lim::max()));
    ASSERT_EQUAL(ll_lim::max() + BigInt(ll_lim::min()), -1);

    ASSERT_EQUAL(BigInt("000000000000000000000000000000000000000001") +
                 BigInt("000000000000000000000000000000000000000001"), 2);
    ASSERT_EQUAL(BigInt("000000000000000000000000000000000000000001") +
                 BigInt("-000000000000000000000000000000000000000001"), 0);

    ASSERT_EQUAL(BigInt("100000000000000000000000000000000000000000") +
                 BigInt("100000000000000000000000000000000000000000"),
                 BigInt("200000000000000000000000000000000000000000"));
}

void TestSub() {
    ASSERT_EQUAL(BigInt(0) - BigInt(0), 0);
    ASSERT_EQUAL(-BigInt(0) + BigInt(0), 0);
    ASSERT_EQUAL(BigInt(1) - 0, 1);

    ASSERT_EQUAL(2 - BigInt(1), BigInt(1));
    ASSERT_EQUAL(BigInt(1) - (-BigInt(1)), BigInt(2));

    ASSERT_EQUAL(llu_lim::max() - BigInt(ll_lim::max()) - BigInt(ll_lim::max()), 1);
    ASSERT_EQUAL(-BigInt(ll_lim::max()) + BigInt(llu_lim::max()) - BigInt(ll_lim::max()), 1);
    ASSERT_EQUAL(-ll_lim::max() - BigInt(ll_lim::max()) + BigInt(llu_lim::max()), 1);
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
    ASSERT_EQUAL(2 * BigInt(2), BigInt(4));
    ASSERT_EQUAL(-2 * BigInt(2), -4);
    ASSERT_EQUAL(BigInt(-2) * -2, 4);

    ASSERT_EQUAL(BigInt(ll_lim::max()) * 2 + 1, BigInt(llu_lim::max()));
    ASSERT_EQUAL(BigInt(llu_lim::max()) - BigInt(ll_lim::max()) * 2, 1);

    ASSERT_EQUAL(BigInt("11111111111111111111111111111") * 2,
                 BigInt("22222222222222222222222222222"));

    ASSERT_EQUAL(BigInt("10000000000000000001") * BigInt(llu_lim::max()),
                 BigInt("10000000000000000000") * BigInt(llu_lim::max()) + BigInt(llu_lim::max()));

    ASSERT_EQUAL(BigInt("98765432100123456789") * BigInt(BigInt("98765432100123456789")),
                 BigInt("9754610577924096936222542295378750190521"));

    ASSERT_EQUAL(BigInt("10000000000000000000000000000") * 0, 0);
    ASSERT_EQUAL(BigInt("00000000000000000000000000001") * (-1), -1);
}

BigInt factorial(const BigInt& num) {
    return (num > 1) ? (num * factorial(num - 1)) : BigInt(1);
}

BigInt fibonacci(const BigInt& num) {
    BigInt a = 0, b = 1, c = 0, i = 0;
    if (num == 0)
        return a;
    for (i = 2; i <= num; ++i) {
        c = a + b;
        a = b;
        b = c;
    }
    return b;
}

void TestUsage() {
    ASSERT_EQUAL(BigInt("265252859812191058636308480000000"), factorial(BigInt(30)));
    ASSERT_EQUAL(BigInt("110560307156090817237632754212345"), fibonacci(BigInt(155)));
}

}  // namespace

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestIOstream);
    RUN_TEST(tr, TestStringConstructor);
    RUN_TEST(tr, TestIntegerConctructor);
    RUN_TEST(tr, TestBigIntComparison);
    RUN_TEST(tr, TestCopy);
    RUN_TEST(tr, TestMove);
    RUN_TEST(tr, TestUnary);
    RUN_TEST(tr, TestSum);
    RUN_TEST(tr, TestSub);
    RUN_TEST(tr, TestMult);
    RUN_TEST(tr, TestUsage);
}