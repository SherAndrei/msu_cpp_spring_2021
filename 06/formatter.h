#ifndef FORMATTER_H
#define FORMATTER_H

#include <string_view>
#include <vector>
#include <string>
#include <utility>

#include "convert.h"

class Formatter {
 public:
    template<class... Args>
    explicit Formatter(const Args&... args)
        : args_(convert(args...)) {}

    std::pair<size_t, size_t> find_next_brackets(std::string_view sv) const;
    size_t parse_argument(std::string_view sv) const;
    std::string get_argument(size_t idx) const;

 private:
    void left_strip(std::string_view& sv) const;
    std::string_view read_token(std::string_view& sv) const;
    bool is_end_of_token(char ch) const;

 private:
    std::vector<std::string> args_;
};

#endif  // FORMATTER_H
