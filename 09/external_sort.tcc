#ifndef EXTERNAL_SORT_TCC
#define EXTERNAL_SORT_TCC

#include <algorithm>

#include "external_sort.h"

template<class Cmp>
Sorter<Cmp>::TempDirectory::TempDirectory() {
    std::filesystem::create_directory(dir_name);
}
template<class Cmp>
Sorter<Cmp>::TempDirectory::~TempDirectory() {
    std::filesystem::remove_all(dir_name);
}

template<class Cmp>
Sorter<Cmp>::Worker::Worker(Sorter* s)
    : _ps(s), _thread([this] {
    _chunk = std::make_unique<uint64_t[]>(CHUNK_SIZE);
    uint64_t current;
    size_t curr_size = 0;
    while (_ps->_inp >> current) {
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
        std::copy_n(_chunk.get(), curr_size, out_iter{_ps->_out, " "});
}) {}

template<class Cmp>
Sorter<Cmp>::Worker::~Worker() {
    _thread.join();
}

template<class Cmp>
void Sorter<Cmp>::Worker::sort_to_temp_file(size_t curr_size) {
    std::sort(_chunk.get(), _chunk.get() + curr_size, _ps->_comp);

    std::fstream temp(TempDirectory::dir_name +
                    std::to_string(_temp_files.size()), IOS_FLAG);
    std::copy_n(_chunk.get(), curr_size, out_iter{temp, " "});

    temp.seekp(0);
    _temp_files.emplace(std::move(temp));
}

template<class Cmp>
std::pair<std::fstream, std::fstream>
Sorter<Cmp>::Worker::front_files() {
    auto left  = std::move(_temp_files.front());
    _temp_files.pop();
    auto right = std::move(_temp_files.front());
    _temp_files.pop();
    return std::make_pair(std::move(left), std::move(right));
}

template<class Cmp>
void Sorter<Cmp>::Worker::merge_temp_files() {
    for (size_t i = 0; _temp_files.size() > 2; i += 2) {
        auto [left, right] = front_files();

        std::fstream temp(TempDirectory::dir_name +
                        std::to_string(i) + std::to_string(i + 1), IOS_FLAG);
        std::merge(
            inp_iter{left},  inp_iter{},
            inp_iter{right}, inp_iter{},
            out_iter{temp, " "}, _ps->_comp
        );
        temp.seekp(0);
        _temp_files.emplace(std::move(temp));
    }
    auto [left, right] = front_files();

    std::merge(
        inp_iter{left},  inp_iter{},
        inp_iter{right}, inp_iter{},
        out_iter{_ps->_out, " "}, _ps->_comp
    );
}

template<class Cmp>
Sorter<Cmp>::Sorter(
    const std::string& inp_filename,
    const std::string& out_filename,
    Cmp comp)
    : _inp(inp_filename)
    , _out(out_filename)
    , _comp(comp) {
    if (!_inp || !_out)
        throw FileError("Cannot open file");
}

template<class Cmp>
void Sorter<Cmp>::external_sort() {
    if (is_empty(_inp))
        return;
    Worker w{this};
}

template<class Cmp>
bool Sorter<Cmp>::is_empty(std::ifstream& inp) const {
    return inp.peek() == std::ifstream::traits_type::eof();
}


#endif  // EXTERNAL_SORT_TCC
