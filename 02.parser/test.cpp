#include "parser.h"
#include "test_runner.h"

void TestInteger();
void TestString();
void TestStartAndEnd();
void TestUsage();
void TestNullptr();

void TestInteger() {
    {
        TokenParser pr;
        bool is_not_a_number = true;

        pr.set_number_callback([&] (uint64_t) {
            is_not_a_number = false;
        });

        for (auto& str : {"", " ", "\n", "\t", " \n\t",
                          "a", "i am not a number\n",
                          "a100", "100a", "10a0",
                          "1e 2q\n4r\te23 \n\t2r5", "19000000000000000000"}) {
            pr.run(str);
            ASSERT(is_not_a_number);
        }
    }
    {
        TokenParser pr;
        uint64_t result = 0ul;

        pr.set_number_callback([&](uint64_t num) {
            result = num;
        });
        std::map<std::string, uint64_t> expected = {
            {"1", 1},
            {"\t1\t", 1},
            {" 1 ", 1},
            {"\n1\n", 1},
            {"123", 123},
            {"0001", 1},
            {"18446744073709551615", ~0}
        };
        for (auto&& [str, res] : expected) {
            pr.run(str);
            ASSERT_EQUAL(result, res);
            result = 0ul;
        }
    }
    {
        TokenParser tp;
        std::vector<uint64_t> resv;
        tp.set_number_callback([&](uint64_t num) {
            resv.push_back(num);
        });
        std::map<std::string, std::vector<uint64_t>> expected = {
            {"1", {1}},
            {"\t1\t2", {1, 2}},
            {" 1 3\n4\t5\n 3", {1, 3, 4, 5, 3}},
            {"123 a 23 q 523", {123, 23, 523}},
            {"qwewqe 10000 00ewq00 0001", {10000, 1}},
            {"18446a744073 18446744073709551615 70955q1615 184467440737095516151", {~0ul}}
        };
        for (auto&& [str, res] : expected) {
            tp.run(str);
            ASSERT_EQUAL(resv, res);
            resv.clear();
        }
    }
}

void TestString() {
    TokenParser pr;
    std::vector<std::string> res;
    pr.set_string_callback([&](const std::string& s){
        res.push_back(s);
    });

    std::map<std::string, std::vector<std::string>> expected = {
        {"", {}},
        {" ", {}},
        {"\n", {}},
        {"\t", {}},
        {" \n\t", {}},
        {"a 1 a,", {"a", "a,"}},
        {"a a 1 ,", {"a", "a", ","}},
        {"a1 1a\t1a1\na1a", {"a1", "1a", "1a1", "a1a"}}
    };

    for (auto&& [str, exp] : expected) {
        pr.run(str);
        ASSERT_EQUAL(res, exp);
        res.clear();
    }
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

void TestNullptr() {
    {
        TokenParser tp;
        tp.set_start_callback(nullptr);
        tp.set_end_callback(nullptr);
        tp.run("1");
        ASSERT(true);
    }
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestNullptr);
    RUN_TEST(tr, TestStartAndEnd);
    RUN_TEST(tr, TestInteger);
    RUN_TEST(tr, TestString);
    RUN_TEST(tr, TestUsage);
}
