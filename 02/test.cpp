#include "parser.h"
#include "test_runner.h"

void TestInteger();
void TestString();
void TestMixed();

void TestInteger() {
    {
        Parser pr;
        bool is_not_a_number = true;

        pr.set_number_callback([&] (uint64_t) {
            is_not_a_number = false;
        });

        pr.run("");
        ASSERT(is_not_a_number);

        pr.run("\n\t");
        ASSERT(is_not_a_number);

        pr.run("i am not a number\n");
        ASSERT(is_not_a_number);

        pr.run("a100");
        ASSERT(is_not_a_number);

        pr.run("100a");
        ASSERT(is_not_a_number);

        pr.run("10a0");
        ASSERT(is_not_a_number);
    }
    {
        Parser pr;
        bool is_number = true;

        pr.set_string_callback([&](std::string_view) {
            is_number = false;
        });

        pr.run("\t1\t");
        ASSERT(is_number);
        pr.run(" 1 ");
        ASSERT(is_number);
        pr.run("\n1\n");
        ASSERT(is_number);

        pr.run("123");
        ASSERT(is_number);

        pr.run("0x1");
        ASSERT(is_number);
    }
}

void TestString() {

}

void TestMixed() {

}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestInteger);
    RUN_TEST(tr, TestString);
    RUN_TEST(tr, TestMixed);
}
