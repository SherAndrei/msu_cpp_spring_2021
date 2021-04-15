#include <iostream>
#include <string>
#include <utility>
#include <algorithm>
#include <charconv>

#include "bigint.h"
#include "biginterr.h"

namespace {

std::string_view ReadToken(std::string_view& sv) {
    while (!sv.empty() && std::isspace(sv[0]))
        sv.remove_prefix(1);

    auto pos = sv.find_first_of(" \t\n");
    auto result = sv.substr(0, pos);
    sv.remove_prefix(pos != sv.npos ? pos : sv.size());
    return result;
}

bool IsEndOfToken(char ch) {
    return (ch == '\0' || std::isdigit(ch));
}

void Parse(std::string_view sv, SimpleVector<uint64_t>& data) {
    uint64_t number;
    while (!sv.empty()) {
        if (auto token = ReadToken(sv);
            !token.empty()) {
            auto [ptr, errc] = std::from_chars(token.begin(), token.end(), number);
            (errc == std::errc() && IsEndOfToken(*ptr))
                ? data.push_back(number)
                : throw ParsingError("Cannot parse number: " + std::string(sv));
        }
    }
}

}  // namespace


BigInt::BigInt(std::string_view sv) {
    if (sv[0] == '-') {
        sign_ = true;
        sv.remove_prefix(1);
    }
    while (!sv.empty()) {
        size_t try_base = std::min(_BASE_NDIGITS_, sv.size());
        Parse(sv.substr(0, try_base), data_);
        sv.remove_prefix(try_base);
    }
}

BigInt& BigInt::operator=(std::string_view sv) {
    (void) sv;
    return *this;
}

std::string BigInt::to_string() const {
    std::string res;
    if (sign_)
        res += '-';
    for (size_t block : data_)
        res += std::to_string(block);
    return res;
}

std::istream& operator>>(std::istream& is, BigInt& bnum) {
    std::string snum;
    is >> snum;
    bnum = BigInt(snum);
    return is;
}
std::ostream& operator<<(std::ostream& os, const BigInt& bnum) {
    return os << bnum.to_string();
}
