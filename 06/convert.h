#ifndef CONVERT_H
#define CONVERT_H

#include <vector>
#include <string>
#include <string_view>
#include <sstream>

template<class... Args>
std::vector<std::string> convert() {
    return {};
}

template<class T>
std::vector<std::string> convert(const T& arg) {
    if constexpr (std::is_convertible_v<T, std::string_view>) {
        return std::vector<std::string>{std::string(arg)};
    } else if constexpr (std::is_integral_v<T>) {
        return std::vector<std::string>{std::to_string(arg)};
    } else {
        std::ostringstream os;  // to_chars for floating points is unimplimented :(
        os << arg;
        return std::vector<std::string>{std::string(os.str())};
    }
}

template<class T, class... Args>
std::vector<std::string> convert(const T& arg, const Args&... args) {
    std::vector<std::string> result = convert(arg);
    std::vector<std::string> remaining = convert(args...);
    result.insert(
        result.end(),
        std::make_move_iterator(remaining.begin()),
        std::make_move_iterator(remaining.end())
    );
    return result;
}

#endif  // CONVERT_H
