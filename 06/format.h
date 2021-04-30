#ifndef FORMAT_H
#define FORMAT_H

#include <string>
#include <string_view>
#include <vector>
#include <charconv>

#include "convert.h"
#include "formaterr.h"

using std::string_view;
using std::string;

string_view read_token(string_view& sv);
size_t parse_brackets(string_view brackets);

string_view read_token(string_view& sv) {
    while (!sv.empty() && std::isspace(sv[0]))
        sv.remove_prefix(1);

    auto pos = sv.find_first_of(" \t\n");
    auto result = sv.substr(0, pos);
    sv.remove_prefix(pos != sv.npos ? pos : sv.size());
    return result;
}

size_t parse_brackets(string_view brackets) {
    string_view token = read_token(brackets);
    string_view unexp_token = read_token(brackets);

    if (!unexp_token.empty())
        throw ArgumentError("Unexpected bracket token");

    size_t number;
    auto [ptr, errc] = std::from_chars(token.begin(), token.end(), number);

    if (errc == std::errc())
        return number;

    throw ArgumentError("Incorrect argument");
}


template<class... Args>
string format(string_view fmt, const Args&... args) {
    std::vector<string> str_args = convert(args...);

    string fmted;
    while (!fmt.empty()) {
        size_t left = fmt.find('{');
        size_t right = fmt.find('}');

        if (right < left) {
            throw BracesError("Unexpected right bracket");
        }
        if (left == right) {  // == npos
            return fmted + string(fmt);
        }

        string_view inner = fmt.substr(left + 1, right - left - 1);
        size_t idx = parse_brackets(inner);
        if (idx >= str_args.size())
            throw BracesError("Incorrect index");

        fmted += string(fmt.substr(0, left)) + str_args[idx];

        fmt.remove_prefix(right + 1);
    }

    return fmted;
}

#endif  // FORMAT_H
