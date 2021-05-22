#include <fstream>
#include <random>
#include <functional>
#include <string>
#include <string_view>

#include "test_runner.h"
#include "external_sort.h"
#include "profile.h"

namespace {

template<typename Compare>
bool isSorted(const std::string& filename, Compare comp) {
    std::ifstream file(filename);
    if (!file)
        throw FileError("Cannot open file");
    uint64_t prev, curr;
    if (!(file >> prev))
        return true;

    while (file >> curr) {
        if (!comp(prev, curr))
            return false;
        prev = curr;
    }
    return true;
}

template<class Compare>
void doTest(
    const std::string& inp_filename,
    const std::string& out_filename,
    Compare comp
) {
    std::ifstream input(inp_filename);
    std::ofstream output(out_filename);

    Sorter sorter{inp_filename, out_filename, comp};
    sorter.external_sort();
    ASSERT(isSorted(out_filename, comp));
}

void doAllTests(
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
        for (auto&& val : values)
            inp <<  val << ' ';
    };
    static const std::string inp_filename = "tests/small.txt";
    static const std::string out_filename = "tests/output.txt";
    {
        create_and_fill(inp_filename, {0ul});
        doAllTests(inp_filename, out_filename);
    }
    {
        create_and_fill(inp_filename, {1ul, 1ul, 1ul});
        doAllTests(inp_filename, out_filename);
    }
    {
        create_and_fill(inp_filename, {0ul, 1ul, 2ul, 3ul});
        doAllTests(inp_filename, out_filename);
    }
    {
        create_and_fill(inp_filename, {4ul, 3ul, 2ul, 1ul});
        doAllTests(inp_filename, out_filename);
    }
}

void fill(
    const std::string& filename,
    size_t size_in_bytes
) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint64_t> distrib(0, std::numeric_limits<uint64_t>::max());

    size_t curr_size = 0;
    std::ofstream file(filename);

    while (true) {
        std::string num = std::to_string(distrib(gen));
        std::string_view num_view(num);
        while (!num_view.empty()) {
            if (++curr_size == size_in_bytes) return;
            file << num_view[0];
            num_view.remove_prefix(1);
        }
        if (++curr_size == size_in_bytes) return;
        file << ' ';
    }
}

void testLarge() {
    static const std::string out_filename = "tests/output.txt";
    std::map<size_t, std::string> bytes_to_name = {
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
        std::ifstream inp(name);
        std::ofstream out(out_filename);
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
