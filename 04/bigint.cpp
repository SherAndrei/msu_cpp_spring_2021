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

void Parse(std::string_view sv, SimpleVector<Block>& data) {
    uint64_t number;
    while (!sv.empty()) {
        if (auto token = ReadToken(sv);
            !token.empty()) {
            auto [ptr, errc] = std::from_chars(token.begin(), token.end(), number);
            (errc == std::errc() && IsEndOfToken(*ptr))
                ? data.push_back(Block{number})
                : throw ParsingError("Cannot parse number: " + std::string(sv));
        }
    }
}

}  // namespace


BigInt::BigInt(std::string_view sv) {
    if (sv[0] == '-') {
        negative_ = true;
        sv.remove_prefix(1);
    }
    while (!sv.empty()) {
        size_t try_base = std::min(_BASE_NDIGITS_, sv.size());
        Parse(sv.substr(sv.size() - try_base), blocks_);
        sv.remove_suffix(try_base);
    }
    if (negative_ && blocks_.size() == 1 && blocks_[0].number == 0)
        negative_ = false;

    remove_leading_zeros();
}

BigInt& BigInt::operator=(std::string_view sv) {
    return *this = BigInt(sv);
}

std::string BigInt::to_string() const {
    std::string res;
    if (negative_) res += '-';
    bool add_zeros = false;
    std::for_each(blocks_.rbegin(), blocks_.rend(), [&] (Block bl) {
        res += bl.to_string(add_zeros);
        add_zeros = true;
    });

    return res;
}

void BigInt::remove_leading_zeros() {
    while (blocks_.size() > 1 && blocks_.back().number == 0)
        blocks_.pop_back();
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

bool BigInt::operator < (const BigInt& rhs) const {
    return (rhs.negative_ <= negative_ &&
            std::lexicographical_compare(blocks_.rbegin(), blocks_.rend(),
                rhs.blocks_.rbegin(), rhs.blocks_.rend(), [this] (Block lhs, Block rhs) {
                    return (lhs.number < rhs.number) ^ negative_;
                }));
}

// constexpr BigInt BigInt::operator+(const BigInt&) const {
//     return *this;
// }
// constexpr BigInt BigInt::operator-(const BigInt&) const {
//     return *this;
// }
// constexpr BigInt BigInt::operator*(const BigInt&) const {
//     return *this;
// }
// constexpr BigInt& BigInt::operator+=(const BigInt&) {
//     return *this;
// }
// constexpr BigInt& BigInt::operator-=(const BigInt&) {
//     return *this;
// }
// constexpr BigInt& BigInt::operator*=(const BigInt&) {
//     return *this;
// }
