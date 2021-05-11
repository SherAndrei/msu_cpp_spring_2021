#ifndef CONVERT_H
#define CONVERT_H

#include <string>
#include <string_view>
#include <sstream>

template<class T>
std::string convert(const T& arg) {
    if constexpr (std::is_convertible_v<T, std::string_view>) {
        return std::string(arg);
    } else if constexpr (std::is_integral_v<T>) {
        return std::to_string(arg);
    } else {
        std::ostringstream os;  // to_chars is not implimented for floating points  :(
        os << arg;
        return os.str();
    }
}

#endif  // CONVERT_H
