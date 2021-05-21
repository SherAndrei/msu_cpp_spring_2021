#include <fstream>
#include <random>

#include "test_runner.h"
#include "external_sort.h"

namespace {

template<typename Compare>
bool isSorted(std::ifstream& file, Compare comp) {
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
    {
        std::ifstream input(inp_filename);
        std::ofstream output(out_filename);

        Sorter sorter(comp);
        sorter.external_sort(input, output);
    }
    {
        std::ifstream output(out_filename);
        ASSERT(isSorted(output, comp));
    }
}


void testEmpty() {
    doTest(
        "tests/empty.txt",
        "tests/output.txt",
        std::less{}
    );
}

void testSmall() {

}

}  // namespace

int main() {
    TestRunner tr;
    RUN_TEST(tr, testEmpty);
    RUN_TEST(tr, testSmall);
}
