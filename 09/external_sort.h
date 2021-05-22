#ifndef EXTERNAL_SORT_H
#define EXTERNAL_SORT_H

#include <string>
#include <fstream>
#include <algorithm>
#include <thread>
#include <memory>
#include <filesystem>
#include <queue>
#include <utility>
#include <iterator>

#include "extsorterr.h"

inline constexpr size_t MEMORY_BOUNDARY = 8388608;

template<class Compare>
class Sorter {
 private:
    struct TempDirectory {
        static inline const std::string dir_name = "__temp/";

        TempDirectory() {
            std::filesystem::create_directory(dir_name);
        }
        ~TempDirectory() {
            std::filesystem::remove_all(dir_name);
        }
    };

 private:
    using inp_iter = std::istream_iterator<uint64_t>;
    using out_iter = std::ostream_iterator<uint64_t>;

    static constexpr std::ios::openmode IOS_FLAG = std::ios::trunc | std::ios::out | std::ios::in;

 private:
    static constexpr unsigned POOL_SIZE = 2u;
    static constexpr unsigned THREAD_MEM = MEMORY_BOUNDARY / POOL_SIZE;

    static constexpr unsigned CHUNK_SIZE = MEMORY_BOUNDARY / sizeof(uint64_t);

 public:
    explicit Sorter(
        const std::string& inp_filename,
        const std::string& out_filename,
        Compare comp)
        : _inp(inp_filename)
        , _out(out_filename)
        , _comp(comp) {
        if (!_inp || !_out)
            throw FileError("Cannot open file");
    }

    void external_sort() {
        if (is_empty(_inp))
            return;

        _chunk = std::make_unique<uint64_t[]>(CHUNK_SIZE);
        uint64_t current;
        size_t curr_size = 0;
        while (_inp >> current) {
            _chunk[curr_size++] = current;
            if (curr_size == CHUNK_SIZE) {
                sort_to_temp_file(curr_size);
                curr_size = 0;
            }
        }
        if (curr_size > 0) {
            sort_to_temp_file(curr_size);
        }

        if (_temp_files.size() > 1)
            merge_temp_files();
        else
            std::copy_n(_chunk.get(), curr_size, out_iter{_out, " "});
    }

 private:
    void sort_to_temp_file(size_t curr_size) {
        std::sort(_chunk.get(), _chunk.get() + curr_size, _comp);

        std::fstream temp(TempDirectory::dir_name +
                          std::to_string(_temp_files.size()), IOS_FLAG);
        std::copy_n(_chunk.get(), curr_size, out_iter{temp, " "});

        temp.seekp(0);
        _temp_files.emplace(std::move(temp));
    }

    std::pair<std::fstream, std::fstream>
    front_files() {
        auto left  = std::move(_temp_files.front());
        _temp_files.pop();
        auto right = std::move(_temp_files.front());
        _temp_files.pop();
        return std::make_pair(std::move(left), std::move(right));
    }

    void merge_temp_files() {
        for (size_t i = 0; _temp_files.size() > 2; i += 2) {
            auto [left, right] = front_files();

            std::fstream temp(TempDirectory::dir_name +
                              std::to_string(i) + std::to_string(i + 1), IOS_FLAG);
            std::merge(
                inp_iter{left},  inp_iter{},
                inp_iter{right}, inp_iter{},
                out_iter{temp, " "}, _comp
            );
            temp.seekp(0);
            _temp_files.emplace(std::move(temp));
        }
        auto [left, right] = front_files();

        std::merge(
            inp_iter{left},  inp_iter{},
            inp_iter{right}, inp_iter{},
            out_iter{_out, " "}, _comp
        );
    }

 private:
    std::unique_ptr<uint64_t[]> _chunk = nullptr;
    std::queue<std::fstream> _temp_files;

 private:
    bool is_empty(std::ifstream& inp) const {
        return inp.peek() == std::ifstream::traits_type::eof();
    }

 private:
    std::ifstream _inp;
    std::ofstream _out;
    const Compare _comp;
    TempDirectory temp;
    // std::array<std::thread, POOL_SIZE> threads_;
};

#endif  // EXTERNAL_SORT_H
