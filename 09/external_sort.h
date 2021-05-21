#ifndef EXTERNAL_SORT_H
#define EXTERNAL_SORT_H

#include <string>
#include <fstream>
#include <functional>
#include <algorithm>
#include <thread>
#include <array>
#include <memory>
#include <filesystem>
#include <vector>
#include <utility>

#include "extsorterr.h"

// inline constexpr size_t MEMORY_BOUNDARY = 8388608;
inline constexpr size_t MEMORY_BOUNDARY = 4 * sizeof(uint64_t);

template<class Compare>
class Sorter {
 private:
    struct TempDirectory {
        static inline const std::string tempdir_name = "__temp/";

        TempDirectory() {
            std::filesystem::create_directory(tempdir_name);
        }
        ~TempDirectory() {
            // std::filesystem::remove_all(tempdir_name);
        }
    };

 private:
    static constexpr unsigned POOL_SIZE = 2u;
    static constexpr unsigned THREAD_MEM = MEMORY_BOUNDARY / POOL_SIZE;

 public:
    explicit Sorter(
        const std::string& inp_filename,
        const std::string& out_filename,
        Compare comp)
        : _inp(inp_filename)
        , _inp_size(std::filesystem::file_size(inp_filename))
        , _out(out_filename)
        , _comp(comp) {
        if (!_inp || !_out)
            throw FileError("Cannot open file");
    }

    void external_sort() {
        if (is_empty(_inp))
            return;

        TempDirectory temp;

        _n_available_ints = MEMORY_BOUNDARY / sizeof(uint64_t);
        _chunk = std::make_unique<uint64_t[]>(_n_available_ints);

        // кладем все в файлы
        size_t n_minichunks = 0;
        while (_inp) {
            sort_to_file(TempDirectory::tempdir_name + std::to_string(n_minichunks));
            n_minichunks++;
        }

        // собираем инфу из файлов в чанк
        //  V--V        V--V             V--V          V-------V
        // [________], [________], .... [_______] --> {_________}
        size_t finished_files = 0;
        size_t n_in_chunk = 0;
        size_t n_minichunk_ints = _n_available_ints / n_minichunks;
        while (true) {
            if (finished_files >= n_minichunks)
                break;
            for (size_t i = 0; i < n_minichunks; i++) {
                auto& in = _temp_files[i];
                if (!in) {
                    finished_files++;
                    continue;
                }
                for (size_t j = 0; j < n_minichunk_ints; j++, n_in_chunk++) {
                    in >> _chunk[n_minichunk_ints * i + j];
                    if (!in) break;
                }
            }

            // сортируем чанк и выплевываем его в _out
            std::sort(_chunk.get(), _chunk.get() + n_in_chunk, _comp);

            for (size_t i = 0; i < n_in_chunk; i++) {
                _out << _chunk[i] << ' ';
            }
            n_in_chunk = 0;
        }
    }

 private:
    void sort_to_file(const std::string& filename) {
        size_t n_ints = 0;
        for (; n_ints < _n_available_ints && _inp >> _chunk[n_ints]; n_ints++) {}

        std::sort(_chunk.get(), _chunk.get() + n_ints);
        std::fstream tempfile(filename, std::ios::in | std::ios::out | std::ios::trunc);
        for (size_t i = 0; i < n_ints && tempfile << _chunk[i] << ' '; i++) {}

        tempfile.seekp(0);
        _temp_files.emplace_back(std::move(tempfile));
    }

 private:
    size_t _n_available_ints = 0;
    std::unique_ptr<uint64_t[]> _chunk = nullptr;
    std::vector<std::fstream> _temp_files;

 private:
    bool is_empty(std::ifstream& inp) const {
        return inp.peek() == std::ifstream::traits_type::eof();
    }

 private:
    std::ifstream _inp;
    const size_t _inp_size;
    std::ofstream _out;
    const Compare _comp;
    // std::array<std::thread, POOL_SIZE> threads_;
};

#endif  // EXTERNAL_SORT_H
