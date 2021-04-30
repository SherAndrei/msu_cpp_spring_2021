#include <iostream>

#include "format.h"
#include "formaterror.h"
#include "test_runner.h"

void TestConvert();
void TestValid();
void TestIncorrectBrackets();
void TestInvalidArguments();

void TestConvert() {
    ASSERT_EQUAL(std::vector<std::string>({}), convert());
    ASSERT_EQUAL(std::vector<std::string>({"1"}), convert(1));
    ASSERT_EQUAL(std::vector<std::string>({"1", "2", "3"}), convert(1, 2, 3));
    ASSERT_EQUAL(std::vector<std::string>({"1", "2", "3"}), convert("1", "2", "3"));
    ASSERT_EQUAL(std::vector<std::string>({"abc", "2", "0"}), convert("abc", 2, false));
}

void TestValid() {
    std::string text = format("{1} + {2} != {0}", 3.001, "one", 2);
    ASSERT_EQUAL(text, "one + 2 != 3.001");

    text = format("{  1}+{ 1 } = {0   } just stuff", 2, "one");
    ASSERT(text == "one+one = 2 just stuff");

    text = format("Just unformatted string");
    ASSERT(text == "Just unformatted string")

    text = format("{3}{0}{1}{2}", "@", "#", "$", "%");
    ASSERT(text == "%@#$");
}

void TestIncorrectBrackets() {
    std::map<std::string_view, std::pair<int, int>> test = {
        {"{1}+{1{} = {0}", {1, 2}},
        {"{1}+{1} }} = {0}", {3, 2}},
        {"some } text {0}", {5, 3}},
        {"some { text {0}", {3, 4}}
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
        {"text with { 0tr0uble } in brace", "arg"},
        {"text with { not-a-number } in brace", "arg"},
        {"text with { 99999999999999999999999999999 } in brace", "arg"},
        {"text with { -1 } in brace", "arg"},
        {"text with { 3 } in brace", "arg"}
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
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestConvert);
    RUN_TEST(tr, TestIncorrectBrackets);
    RUN_TEST(tr, TestInvalidArguments);
    RUN_TEST(tr, TestValid);
}
