#ifndef EXTERNAL_SORT_H
#define EXTERNAL_SORT_H

#include <fstream>
#include <functional>
#include <algorithm>
#include <thread>
#include <array>

#include "extsorterr.h"

inline constexpr size_t MEMORY_BOUNDARY = 8388608;

template<class Compare>
class Sorter {
 private:
    static constexpr unsigned POOL_SIZE = 2u;

 public:
    explicit Sorter(Compare comp)
        : _comp(comp) {}

    template<typename RandomIt>
    void sort(RandomIt begin, RandomIt end) const {
        std::sort(begin, end, _comp);
    }

    void external_sort(std::ifstream& input, std::ofstream& output) {
        if (!input || !output)
            throw FileError("Cannot open file");
        if (is_empty(input))
            return;
        std::vector<uint64_t> values;
        uint64_t val;
        while (input >> val)
            values.push_back(val);
        sort(values.begin(), values.end());
        for (auto&& value : values)
            output << value << ' ';
    }

 private:
    bool is_empty(std::ifstream& inp) const {
        return inp.peek() == std::ifstream::traits_type::eof();
    }

 private:
    Compare _comp;
    std::array<std::thread, POOL_SIZE> threads_;
};

#endif  // EXTERNAL_SORT_H
