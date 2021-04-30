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

bool is_end_of_token(char ch);
void left_strip(string_view& sv);
string_view read_token(string_view& sv);
size_t parse_brackets(string_view brackets);

void left_strip(string_view& sv) {
    while (!sv.empty() && std::isspace(sv[0]))
        sv.remove_prefix(1);
}

string_view read_token(string_view& sv) {
    left_strip(sv);

    auto pos = sv.find_first_of(" \t\n");
    auto result = sv.substr(0, pos);
    sv.remove_prefix(pos != sv.npos ? pos : sv.size());
    return result;
}

bool is_end_of_token(char ch) {
    return (std::isspace(ch) || ch == '}');
}

size_t parse_brackets(string_view brackets) {
    string_view token = read_token(brackets);

    size_t number;
    auto [ptr, errc] = std::from_chars(token.begin(), token.end(), number);

    if (is_end_of_token(*ptr) && errc == std::errc())
        return number;
    throw ArgumentError("Invalid argument");
}


template<class... Args>
string format(string_view fmt, const Args&... args) {
    std::vector<string> str_args = convert(args...);

    string fmted;
    while (!fmt.empty()) {
        size_t left = fmt.find('{');
        size_t right = fmt.find('}');

        if (right < left) {
            throw BracketsError("Unexpected right bracket");
        }
        if (left == right) {  // == npos
            return fmted + string(fmt);
        }

        string_view inner = fmt.substr(left + 1, right - left - 1);
        if (inner.find('{') != fmt.npos)
            throw BracketsError("Unexpected left bracket");

        size_t idx = parse_brackets(inner);
        if (idx >= str_args.size())
            throw ArgumentError("Incorrect index");

        fmted += string(fmt.substr(0, left)) + str_args[idx];

        fmt.remove_prefix(right + 1);
    }

    return fmted;
}

#endif  // FORMAT_H
