#include <iostream>

#include "format.h"
#include "test_runner.h"

void TestValid();
void TestIncorrectBraces();
void TestIncorrectArguments();

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

void TestIncorrectBraces() {
    try {
        try {
            auto text = format("{1}+{1{} = {0}", 2, "one");
            ASSERT(false);
        } catch (const BracesError& fe) {
            ASSERT(true);
        }
        try {
            auto text = format("{1}+{1} }} = {0}", 2, "one");
            ASSERT(false);
        } catch (const BracesError& fe) {
            ASSERT(true);
        }
        try {
            auto text = format("some } text {0}", "some string");
            ASSERT(false);
        } catch (const BracesError &fe) {
            ASSERT(true);
        }
        try {
            auto text = format("some { text {0}", "some string");
            ASSERT(false);
        } catch (const BracesError &fe) {
            ASSERT(true);
        }
    } catch (const FormatError& er) {
        std::cerr << er.what() << std::endl;
        ASSERT(false);
    }
}

void TestIncorrectArguments() {
    try {
        for (auto&& arg : {"text with empty {} in brace",
            "text with only spaces {     } in brace"
        }) {
            try {
                auto text = format(arg);
                ASSERT(false);
            } catch (const ArgumentError &fe) {
                ASSERT(true);
            }
        }
        std::map<std::string_view, std::string_view> one_arg_test = {
            {"text with { 0tr0uble } in brace", "arg"},
            {"text with { not-a-number } in brace", "arg"},
            {"text with { 5 } in brace", "arg"},
            {"text with { -1 } in brace", "arg"}
        };

        for (auto&& [str, arg] : one_arg_test) {
            try {
                auto text = format(str, arg);
                ASSERT(false);
            } catch (const ArgumentError &fe) {
                ASSERT(true);
            }
        }
        try {
            auto text = format("text with { 3 } in brace", 0, 1, 2);
            ASSERT(false);
        } catch (const FormatError &fe) {
            ASSERT(true);
        }
    } catch (const FormatError& er) {
        std::cerr << er.what() << std::endl;
        ASSERT(false);
    }
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestValid);
    RUN_TEST(tr, TestIncorrectBraces);
    RUN_TEST(tr, TestIncorrectArguments);
}
