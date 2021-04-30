#ifndef FORMAT_H
#define FORMAT_H


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

    size_t number;
    auto [ptr, errc] = std::from_chars(token.begin(), token.end(), number);

    token = read_token(brackets);
    if (!brackets.empty())
        throw ArgumentError("Unexpected bracket token");

    if (errc == std::errc() && (*ptr == '\0' || std::isspace(*ptr)))
        return number;

    throw ArgumentError("Incorrect argument");
}


template<class... Args>
string format(string_view fmt, const Args&... args) {
    std::vector<string> str_args = convert(args...);



    // size_t left = fmt.find('{');
    // size_t right = fmt.find('}');

    // if (right < left) {
    //     throw BracesError("Unexpected right bracket");
    // }
    // if (left == right) {  // == npos
    //     return string(fmt);
    // }

    // string_view brackets = fmt.substr(left, right - left);
    // size_t num = parse_brackets(brackets);

    // string fmted = string(fmt.substr(0, left));


    // return fmted;
}

#endif  // FORMAT_H
