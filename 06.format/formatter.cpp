#include <charconv>
#include <algorithm>

#include "formatter.h"
#include "formaterror.h"


std::pair<size_t, size_t> Formatter::find_next_brackets(std::string_view sv) const {
    size_t left = sv.find('{');
    size_t right = sv.find('}');

    if ((right < left) ||
        (right != sv.npos && left == sv.npos))
        throw BracketsError("Unexpected right bracket");

    if ((sv.substr(left + 1, right - left - 1).find('{') != sv.npos) ||
        (right == sv.npos && left != sv.npos))
        throw BracketsError("Unexpected left bracket");

    return {left, right};
}

size_t Formatter::parse_argument(std::string_view sv) const {
    std::string_view token = read_token(sv);
    std::string_view next_token = read_token(sv);

    size_t number;
    auto [ptr, errc] = std::from_chars(token.begin(), token.end(), number);

    if (next_token.empty() && is_end_of_token(*ptr) && errc == std::errc())
        return number;
    throw ArgumentError("Invalid argument");
}

std::string Formatter::get_argument(size_t idx) {
    if (idx >= args_.size())
        throw ArgumentError("Invalid index");
    max_used_idx_ = std::max(max_used_idx_, idx);
    return args_[idx];
}

void Formatter::check_args_usage() const {
    if (args_.size() != 0 && max_used_idx_ < args_.size() - 1)
        throw ArgumentError("More arguments than used");
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
