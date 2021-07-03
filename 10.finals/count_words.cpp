#include <fstream>
#include <iostream>
#include <future>
#include <thread>
#include <mutex>
#include <vector>
#include <string>

/*
Обработать каждую строку файла (подсчитать суммарное количество слов) в несколько потоков.
*/

class Counter {
 private:
    class Worker {
     public:
        explicit Worker(Counter* counter)
            : _pcounter(counter) {
            std::packaged_task<size_t()> task{
                [this] { return this->work(); }
            };
            future = task.get_future();
            _thr = std::thread(std::move(task));
        }

        ~Worker() {
            _thr.join();
        }

        std::future<size_t> future;

     private:
        size_t work() {
            size_t counter = 0;
            while (true) {
                std::string line;
                {
                    std::lock_guard lock{_pcounter->_f_mutex};
                    std::getline(_pcounter->_f, line);
                    if (!_pcounter->_f) break;
                }
                counter += count_words(line);
            }
            return counter;
        }

        size_t count_words(const std::string& line) {
            size_t counter = 0;
            bool is_last_letter = false;
            for (char c : line) {
                if (std::isspace(c)) {
                    if (is_last_letter)
                        counter++;
                    is_last_letter = false;
                } else {
                    is_last_letter = true;
                }
            }
            if (is_last_letter)
                counter++;
            return counter;
        }

     private:
        Counter* _pcounter;
        std::thread _thr;
    };


 public:
    explicit Counter(const std::string& filename) : _f(filename) {
        if (!_f) throw std::runtime_error("cannot open file!");
    }

    size_t count() {
        Worker w1{this};
        Worker w2(this);
        size_t w1sum = w1.future.get();
        size_t w2sum = w2.future.get();
        return w1sum + w2sum;
        return w1sum;
    }

 private:
    std::ifstream _f;
    std::mutex _f_mutex{};
};

int main() {
    Counter c{"input.txt"};
    std::cout << c.count() << std::endl;
}
