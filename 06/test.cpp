#include <iostream>
#include <random>
#include <algorithm>

#include "format.h"
#include "formaterror.h"
#include "test_runner.h"

void TestValid();
void TestIncorrectBrackets();
void TestInvalidArguments();

void TestValid() {
    ASSERT_EQUAL("", format(""));
    ASSERT_EQUAL("Sample text", format("Sample text"));

    ASSERT_EQUAL("example", format("{0}", "example"));
    ASSERT_EQUAL("1", format("{0}", 1));
    ASSERT_EQUAL("2.5", format("{0}", 2.5));

    ASSERT_EQUAL("\n\n\n", format("\n{0}\n", "\n"));
    ASSERT_EQUAL("\tqwerty\n", format("\t{0}erty\n", "qw"));
    ASSERT_EQUAL("\tqwerty\n", format("\tqw{0}ty\n", "er"));
    ASSERT_EQUAL("\tqwerty\n", format("\tqwer{0}\n", "ty"));

    ASSERT_EQUAL(format("{0} {1}", "a", "a"), format("{0} {0}", "a"));

    ASSERT_EQUAL("one + 2 != 3.001", format("{1} + {2} != {0}", 3.001, "one", 2));
    ASSERT_EQUAL("one+one = 2 just stuff", format("{  1}+{ 1 } = {0   } just stuff", 2, "one"));
    ASSERT_EQUAL("%@#$", format("{3}{0}{1}{2}", "@", "#", "$", "%"));

    {
        std::string heavy(1600, '\0');
        for (size_t i = 0; i < heavy.size(); i++) {
            heavy[i] = (i % 4 == 0) ? '{' :
                       (i % 4 == 1) ? '0' :
                       (i % 4 == 2) ? '}' : ' ';
        }
        ASSERT_EQUAL(std::string(800, ' '), format(heavy, " "));
    }
    {
        constexpr size_t args_len = 100;
        std::array<int, args_len> idxs;
        std::iota(idxs.begin(), idxs.end(), 0);

        std::array<int, args_len> args;
        std::iota(args.begin(), args.end(), 0);

        std::random_device rd;
        std::mt19937 g(rd());
        for (int i = 0; i < 5; i++) {
            std::shuffle(args.begin(), args.end(), g);

            std::string to_fmt, expected;
            for (int arg : args) {
                to_fmt += "{" + std::to_string(arg) + "}";
                expected += std::to_string(arg);
            }
            std::string res = std::apply([&to_fmt] (auto ...params) {
                return format(to_fmt, params...);
            }, idxs);
            ASSERT_EQUAL(expected, res);
        }
    }
}

void TestIncorrectBrackets() {
    std::map<std::string_view, std::pair<int, int>> test = {
        {"{1}+{1{} = {0}", {1, 2}},
        {"{1}+{1} }} = {0}", {3, 2}},
        {"some } text {0}", {5, 3}},
        {"some { text {0}", {3, 4}},
        {"sample {0} text {1} {", {3, 4}},
        {"sample {0} text {1} }", {3, 4}},
        {"{ sample {1} text {0}", {3, 4}},
        {"} sample {1} text {0}", {3, 4}},
        {"{{0}}", {0, 0}},
        {"{{{0}}}", {0, 0}},
        {"{1}{0{1}0}{1}", {0, 0}},
    };
    for (auto&& [str, args] : test) {
        try {
            auto text = format(str, args.first, args.second);
            ASSERT(false);
        } catch (const BracketsError& fe) {
            ASSERT(true);
        } catch (const FormatError& fe) {
            std::cerr << str << std::endl;
            ASSERT(false);
        }
    }
}

void TestInvalidArguments() {
    std::map<std::string_view, std::string_view> one_arg_test = {
        {"text with empty {} in brace", ""},
        {"text with only spaces {     } in brace", ""},
        {"text with { 0tr0uble } in brace", "trash"},
        {"text with { not-a-number } in brace", "not-a-num"},
        {"text with { 99999999999999999999999999999 } in brace", "big num"},
        {"text with { -1 } in brace", "neg num"},
        {"text with { 3 } in brace", "out of range num"},
        {"text with { 0 1 } in brace", "two nums"}
    };

    for (auto&& [str, arg] : one_arg_test) {
        try {
            auto text = format(str, arg);
            ASSERT(false);
        } catch (const ArgumentError &fe) {
            ASSERT(true);
        } catch (const FormatError& fe) {
            std::cerr << str << std::endl;
            ASSERT(false);
        }
    }
    try {
        auto text = format("str {0} error {1} args", "with", "lenght", "extra");
        ASSERT(false);
    } catch (const ArgumentError &fe) {
        ASSERT(true);
    } catch (const FormatError& fe) {
        ASSERT(false);
    }
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestIncorrectBrackets);
    RUN_TEST(tr, TestInvalidArguments);
    RUN_TEST(tr, TestValid);
}
