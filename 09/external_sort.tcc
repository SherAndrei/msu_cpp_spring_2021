#ifndef EXTERNAL_SORT_TCC
#define EXTERNAL_SORT_TCC

#include <algorithm>
#include <future>

#include "external_sort.h"

template<class Cmp>
Sorter<Cmp>::TempDirectory::TempDirectory() {
    std::filesystem::create_directory(dir_name);
}
#ifndef DEBUG
template<class Cmp>
Sorter<Cmp>::TempDirectory::~TempDirectory() {
    std::filesystem::remove_all(dir_name);
}
#endif
template<class Cmp>
Sorter<Cmp>::Worker::Worker(unsigned id, Sorter* s, Cmp comp)
    : _worker_id(id), _ps(s), _comp(comp) {
        std::packaged_task<std::fstream()> task{
            [this] { return this->work(); }
        };
        _f = task.get_future();
        _thread = std::thread(std::move(task));
}

template<class Cmp>
std::fstream Sorter<Cmp>::Worker::work() {
    _chunk = std::make_unique<uint64_t[]>(CHUNK_SIZE);
    size_t curr_size = 0;
    size_t current;
    while (true) {
        {
            std::lock_guard lock{_ps->_input_mutex};
            _ps->_inp >> current;
            if (!_ps->_inp)
                break;
            _chunk[curr_size++] = current;
        }
        if (curr_size == CHUNK_SIZE) {
            sort_to_temp_file(curr_size);
            curr_size = 0;
        }
    }
    if (curr_size > 0) {
        sort_to_temp_file(curr_size);
    }

    if (_temp_files.empty())
        return std::fstream(temp_filename() + "__empty", IOS_FLAG);

    if (_temp_files.size() > 1)
        merge_temp_files();

    return std::move(_temp_files.front());
}

template<class Cmp>
Sorter<Cmp>::Worker::~Worker() {
    _thread.join();
}

template<class Cmp>
void Sorter<Cmp>::Worker::sort_to_temp_file(size_t curr_size) {
    std::sort(_chunk.get(), _chunk.get() + curr_size, _comp);

    std::fstream temp(temp_filename()
                    + std::to_string(_temp_files.size()), IOS_FLAG);
    std::copy_n(_chunk.get(), curr_size, out_iter{temp, " "});

    temp.seekp(0);
    _temp_files.emplace(std::move(temp));
}


template<class Cmp>
void Sorter<Cmp>::Worker::merge_temp_files() {
    for (size_t i = 0; _temp_files.size() > 2; i += 2) {
        auto [left, right] = front_files();

        std::fstream temp(temp_filename()
                        + std::to_string(i) + std::to_string(i + 1), IOS_FLAG);
        std::merge(
            inp_iter{left},  inp_iter{},
            inp_iter{right}, inp_iter{},
            out_iter{temp, " "}, _comp
        );
        temp.seekp(0);
        _temp_files.emplace(std::move(temp));
    }
    auto [left, right] = front_files();

    std::fstream merged(temp_filename() + "__merged", IOS_FLAG);
    std::merge(
        inp_iter{left},  inp_iter{},
        inp_iter{right}, inp_iter{},
        out_iter{merged, " "}, _comp
    );
    merged.seekp(0);
    _temp_files.emplace(std::move(merged));
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
std::string Sorter<Cmp>::Worker::temp_filename() const {
    return TempDirectory::dir_name + std::to_string(_worker_id) + '_';
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
#if 0
    std::fstream left, right;
    {
        Worker w1{0, this, _comp};
        Worker w2{1, this, _comp};
        left  = w1._f.get();
        right = w2._f.get();
    }
    std::merge(
        inp_iter{left}, inp_iter{},
        inp_iter{right}, inp_iter{},
        out_iter{_out, " "}, _comp
    );
#else
    std::fstream file;
    {
        Worker w1{0, this, _comp};
        file  = w1._f.get();
    }
    std::copy(inp_iter{file}, inp_iter{}, out_iter{_out, " "});
    _out.flush();
#endif
}

template<class Cmp>
bool Sorter<Cmp>::is_empty(std::ifstream& inp) const {
    return inp.peek() == std::ifstream::traits_type::eof();
}


#endif  // EXTERNAL_SORT_TCC
