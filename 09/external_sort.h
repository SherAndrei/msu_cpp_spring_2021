#ifndef EXTERNAL_SORT_H
#define EXTERNAL_SORT_H

#include <string>
#include <fstream>
#include <thread>
#include <memory>
#include <filesystem>
#include <queue>
#include <utility>
#include <iterator>
#include <future>
#include <mutex>

#include "extsorterr.h"

inline constexpr size_t MEMORY_BOUNDARY = 8388608;

template<class Compare>
class Sorter {
 private:
    struct TempDirectory {
        static inline const std::string dir_name = "__temp/";

        TempDirectory();
        ~TempDirectory();
    };

 private:
    class Worker {
     public:
        explicit Worker(unsigned id, Sorter* ps, Compare comp);
        ~Worker();

        std::future<std::fstream> _f;
     private:
        std::fstream work();

        void sort_to_temp_file(size_t curr_size);
        void merge_temp_files();

        std::pair<std::fstream, std::fstream>
        front_files();

        std::string temp_filename() const;
     private:
        unsigned _worker_id;
        Sorter* _ps;
        const Compare _comp;
        std::unique_ptr<uint64_t[]> _chunk = nullptr;
        std::queue<std::fstream> _temp_files;
        std::thread _thread;
    };

 private:
    using inp_iter = std::istream_iterator<uint64_t>;
    using out_iter = std::ostream_iterator<uint64_t>;

    static constexpr std::ios::openmode IOS_FLAG = std::ios::trunc | std::ios::out | std::ios::in;

 private:
    static constexpr unsigned POOL_SIZE = 2u;
    static constexpr unsigned THREAD_MEM = MEMORY_BOUNDARY / POOL_SIZE;
    static constexpr unsigned CHUNK_SIZE = THREAD_MEM / sizeof(uint64_t);

 public:
    explicit Sorter(
        const std::string& inp_filename,
        const std::string& out_filename,
        Compare comp);

    void external_sort();

 private:
    bool is_empty(std::ifstream& inp) const;

 private:
    std::ifstream _inp;
    std::mutex _input_mutex;
    std::ofstream _out;
    const Compare _comp;
    TempDirectory temp;
};

#include "external_sort.tcc"

#endif  // EXTERNAL_SORT_H
