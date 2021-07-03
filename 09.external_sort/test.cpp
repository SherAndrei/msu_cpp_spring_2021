#include <fstream>
#include <random>
#include <string>
#include <string_view>

#include "test_runner.h"
#include "profile.h"

#include "external_sort.h"

namespace {

template<typename Compare>
inline std::vector<uint64_t>
correct_answer(std::ifstream& inp, Compare comp) {
    std::vector<uint64_t> answer{
        std::istream_iterator<uint64_t>{inp},
        std::istream_iterator<uint64_t>{},
    };
    std::sort(answer.begin(), answer.end(), comp);
    return answer;
}

inline std::vector<uint64_t>
my_answer(std::ifstream& out) {
    return {
        std::istream_iterator<uint64_t>{out},
        std::istream_iterator<uint64_t>{},
    };
}

template<class Compare>
inline void doTest(
    const std::string& inp_filename,
    const std::string& out_filename,
    Compare comp
) {
    Sorter sorter{inp_filename, out_filename, comp};
    sorter.external_sort();
    std::ifstream inp(inp_filename);
    std::ifstream out(out_filename);
    if (!inp || !out)
        throw FileError("Cannot open file");
    ASSERT_EQUAL(correct_answer(inp, comp), my_answer(out));
}

inline void doAllTests(
    const std::string& inp_filename,
    const std::string& out_filename
) {
    doTest(inp_filename, out_filename, std::less{});
    doTest(inp_filename, out_filename, std::less_equal{});
    doTest(inp_filename, out_filename, std::greater{});
    doTest(inp_filename, out_filename, std::greater_equal{});
}

void testEmpty() {
    doAllTests(
        "tests/empty.txt",
        "tests/output.txt"
    );
}

void testSmall() {
    auto create_and_fill = [](const std::string& filename,
                              const std::vector<uint64_t>& values) {
        std::ofstream inp(filename);
        std::copy_n(values.begin(), values.size(),
                    std::ostream_iterator<uint64_t>{inp, " "});
    };
    static const std::string inp_filename = "tests/small.txt";
    static const std::string out_filename = "tests/output.txt";
    const std::vector<std::vector<uint64_t>> sequences = {
        {0ul},
        {1ul, 1ul, 1ul},
        {0ul, 1ul, 2ul, 3ul},
        {4ul, 3ul, 2ul, 1ul}
    };
    for (auto&& seq : sequences) {
        create_and_fill(inp_filename, seq);
        doAllTests(inp_filename, out_filename);
    }
}

inline std::random_device rd;
inline std::mt19937 gen(rd());
inline std::uniform_int_distribution<uint64_t>
             distrib(0, std::numeric_limits<uint64_t>::max());

void fill(const std::string& filename, size_t size_in_bytes) {
    size_t curr_size = 0;
    std::ofstream file(filename);

    while (true) {
        const std::string num = std::to_string(distrib(gen));
        std::string_view num_view(num);
        while (!num_view.empty()) {
            if (++curr_size == size_in_bytes) return;
            file.put(num_view[0]);
            num_view.remove_prefix(1);
        }
        if (++curr_size == size_in_bytes) return;
        file.put(' ');
    }
}

void testLarge() {
    static const std::string out_filename = "tests/output.txt";
    const std::map<size_t, std::string> bytes_to_name = {
        {512, "tests/512B.txt"},
        {1024, "tests/1KB.txt"},
        {4194304, "tests/4MB.txt"},
        {8388608, "tests/8MB.txt"},
        {16777216, "tests/16MB.txt"},
        {33554432, "tests/32MB.txt"},
    };

    for (auto&& [size, name] : bytes_to_name) {
#if 1
        fill(name, size);
#endif
        Sorter s{name, out_filename, std::less{}};
        {
            LOG_DURATION(name);
            s.external_sort();
        }
        doAllTests(name, out_filename);
    }
}

}  // namespace

int main() {
    TestRunner tr;
    RUN_TEST(tr, testEmpty);
    RUN_TEST(tr, testSmall);
    RUN_TEST(tr, testLarge);
}
