#include "parser.h"
#include "test_runner.h"

void TestInteger();
void TestString();
void TestStartAndEnd();
void TestUsage();

void TestInteger() {
    {
        TokenParser pr;
        bool is_not_a_number = true;

        pr.set_number_callback([&] (uint64_t) {
            is_not_a_number = false;
        });

        for (auto& str : {"", "\n\t", "i am not a number\n",
                          "a100", "100a", "10a0",
                          "1e 2q\n4r\te23 \n\t2r5", "18446744073709551620"}) {
            pr.run(str);
            ASSERT(is_not_a_number);
        }
    }
    {
        TokenParser pr;
        bool is_number = true;

        pr.set_string_callback([&](const std::string&) {
            is_number = false;
        });

        for (auto& str : {"\t1\t", " 1 ", "\n1\n", "123",
                          "0001", "1 2\t3\n4 \t\n5 6"}) {
            pr.run(str);
            ASSERT(is_number);
        }
    }
}

void TestString() {
    TokenParser pr;
    std::string res;
    pr.set_string_callback([&](const std::string& s){
        res += s;
    });

    for (auto& str : {"C++", "1979",
                      "Java", "1995",
                      "Python", "1991"}) {
        pr.run(str);
    }
    ASSERT(res == "C++JavaPython");

    res.clear();

    for (auto& str : {"C++ 1979",
                      "Java 1995",
                      "Python 1991"}) {
        pr.run(str);
    }
    ASSERT(res == "C++JavaPython");

    res.clear();

    for (auto& str : {"", " \t\n", "a 1 a,",
                      "a a 1 ,",
                      "1 a a,",
                      "a1 1a\t1a1\na1a"}) {
        pr.run(str);
    }
    ASSERT(res == "aa,aa,aa,a11a1a1a1a");
}

void TestStartAndEnd() {
    auto run_parser_4_times = [&] (const TokenParser& p) {
        for (auto& str : {"1", "2", "3", "4"}) {
            p.run(str);
        }
    };
    {
        TokenParser pr;
        size_t counter = 0u;
        pr.set_start_callback([&]{
            counter++;
        });
        pr.set_end_callback([&]{
            counter++;
        });
        run_parser_4_times(pr);
        ASSERT(counter == 8u);
    }
    {
        TokenParser pr;
        size_t counter = 0u;
        pr.set_start_callback([&]{
            counter++;
        });
        run_parser_4_times(pr);
        ASSERT(counter == 4u);
    }
    {
        TokenParser pr;
        size_t counter = 0u;
        pr.set_end_callback([&]{
            counter++;
        });
        run_parser_4_times(pr);
        ASSERT(counter == 4u);
    }
}

void TestUsage() {
    TokenParser table_parser;
    uint64_t sum;
    std::string result;
    table_parser.set_start_callback([&] {
        sum = 0u;
        result = "Groups:\n";
    });
    table_parser.set_number_callback([&] (uint64_t num) {
        sum += num;
    });
    table_parser.set_string_callback([&] (const std::string& s) {
        result += s + '\n';
    });
    table_parser.set_end_callback([&] {
        result += "Sum: " + std::to_string(sum) + '\n';
    });
    table_parser.run(
        "BD-11 100\t"
        "BD-21 350 "
        "T-Lab-11 1000\n"
        "Back-end-11 450\t\n"
        "Golang-11 100\t\n ");
    ASSERT_EQUAL(result, "Groups:\nBD-11\nBD-21\nT-Lab-11\nBack-end-11\nGolang-11\nSum: 2000\n");
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestStartAndEnd);
    RUN_TEST(tr, TestInteger);
    RUN_TEST(tr, TestString);
    RUN_TEST(tr, TestUsage);
}
