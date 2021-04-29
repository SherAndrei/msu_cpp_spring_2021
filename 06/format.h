#ifndef FORMAT_H
#define FORMAT_H

#include <string>
#include <string_view>
#include <tuple>
#include <charconv>

#include "formaterr.h"

using std::string_view;
using std::string;

template<class... Args>
string format(string_view fmt, const Args&... args) {
    auto read_token = [] (string_view& sv) {
        while (!sv.empty() && std::isspace(sv[0]))
            sv.remove_prefix(1);

        auto pos = sv.find_first_of(" \t\n");
        auto result = sv.substr(0, pos);
        sv.remove_prefix(pos != sv.npos ? pos : sv.size());
        return result;
    };

    auto parse_bracket_token = [] (string_view& token) {
        size_t number;
        auto [ptr, errc] = std::from_chars(token.begin(), token.end(), number);
        if (errc == std::errc() && (*ptr == '\0' || std::isspace(*ptr)))
            return number;
        throw ArgumentError("Incorrect argument");
    };

    string fmted;
    fmted.reserve(fmt.size());

    auto t_args = std::tie(args...);
    constexpr size_t n_args = sizeof...(Args);

    auto check_index = [&n_args] (size_t idx) {
        if (idx >= n_args)
            throw ArgumentError("Index out of range");
    };

    // bool is_brackets_open = false;
    for (size_t pos = 0; pos < fmt.size(); ) {
        size_t left_bracket  = fmt.find('{');
        size_t right_bracket = fmt.find('}');

        if (left_bracket > right_bracket)
            throw FormatError("Unexpected left bracket");
        if (left_bracket == fmt.npos)
            return string(fmt);
        if (right_bracket == fmt.npos)
            throw FormatError("Expected right bracket");

        fmted += string(fmt.substr(pos, left_bracket));

        auto inner = fmt.substr(left_bracket + 1, right_bracket - 1);
        auto bracket_token = read_token(inner);
        size_t idx = parse_bracket_token(bracket_token);
        check_index(idx);

        fmted += std::to_string(std::get<idx>(t_args));

        pos = right_bracket;
    }
    return fmted;
}

#endif  // FORMAT_H
