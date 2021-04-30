#include <charconv>

#include "formatter.h"
#include "formaterror.h"


std::pair<size_t, size_t> Formatter::find_next_brackets(std::string_view sv) const {
    size_t left = sv.find('{');
    size_t right = sv.find('}');

    if (right < left)
        throw BracketsError("Unexpected right bracket");

    if (sv.substr(left + 1, right - left - 1).find('{') != sv.npos)
        throw BracketsError("Unexpected left bracket");

    return {left, right};
}

size_t Formatter::parse_argument(std::string_view sv) const {
    std::string_view token = read_token(sv);

    size_t number;
    auto [ptr, errc] = std::from_chars(token.begin(), token.end(), number);

    if (is_end_of_token(*ptr) && errc == std::errc())
        return number;
    throw ArgumentError("Invalid argument");
}

std::string Formatter::get_argument(size_t idx) const {
    if (idx >= args_.size())
        throw ArgumentError("Invalid index");
    return args_[idx];
}

void Formatter::left_strip(std::string_view& sv) const {
    while (!sv.empty() && std::isspace(sv[0]))
        sv.remove_prefix(1);
}

std::string_view Formatter::read_token(std::string_view& sv) const {
    left_strip(sv);

    auto pos = sv.find_first_of(" \t\n");
    auto result = sv.substr(0, pos);
    sv.remove_prefix(pos != sv.npos ? pos : sv.size());
    return result;
}

bool Formatter::is_end_of_token(char ch) const {
    return (std::isspace(ch) || ch == '}');
}
