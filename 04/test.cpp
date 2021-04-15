#include <sstream>
#include <iostream>

#include "test_runner.h"
#include "bigint.h"
#include "biginterr.h"

void TestIOstream();

void TestIOstream() {
    std::vector<std::string> expected = {
        "0", "-1", "1",
        "-32768", "32767", "65535",
        "-9223372036854775808", "9223372036854775807", "18446744073709551615",
        "92233720368547758081844674407370955161518446744073709551615",
        "-184467440737095516151844674407370955161518446744073709551615"
    };
        for (auto&& str : expected) {
            try {
                std::istringstream is(str);
                BigInt bint;
                is >> bint;
                ASSERT_EQUAL(bint.to_string(), str);
            } catch (ParsingError& p) {
                std::cerr << p.what() << str << std::endl;
            }
        }
        for (auto&& str : expected) {
            std::ostringstream os;
            BigInt bint(str);
            os << bint;
            ASSERT_EQUAL(bint.to_string(), os.str());
        }
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestIOstream);
}
