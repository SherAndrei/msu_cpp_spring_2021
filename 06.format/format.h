#ifndef FORMAT_H
#define FORMAT_H

#include <string>
#include <string_view>

#include "formatter.h"

template<class... Args>
std::string format(std::string_view fmt, const Args&... args) {
    Formatter fmter(args...);
    std::string fmted;

    while (!fmt.empty()) {
        auto [left, right] = fmter.find_next_brackets(fmt);
        if (left == right)  // == npos
            break;

        size_t idx = fmter.parse_argument(fmt.substr(left + 1, right - left - 1));
        fmted += std::string(fmt.substr(0, left)) + fmter.get_argument(idx);

        fmt.remove_prefix(right + 1);
    }

    fmter.check_args_usage();
    return fmted + std::string(fmt);
}

#endif  // FORMAT_H
