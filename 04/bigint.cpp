#include <iostream>
#include <string>
#include <utility>
#include <algorithm>
#include <charconv>
#include <ranges>

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

namespace rng = std::ranges;

std::string BigInt::to_string() const {
    std::string res;
    if (negative_) res += '-';
    bool add_zeros = false;
    rng::for_each(rng::reverse_view(blocks_), [&] (Block bl) {
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
            rng::lexicographical_compare(rng::reverse_view(blocks_),
                rng::reverse_view(rhs.blocks_), [this] (Block lhs, Block rhs) {
                    return (lhs.number < rhs.number) ^ negative_;
                }));
}

bool BigInt::operator == (const BigInt& rhs) const {
    return (rhs.negative_ == negative_ &&
            rng::equal(blocks_, rhs.blocks_));
}

BigInt BigInt::operator-() const {
    BigInt res{*this};
    res.negative_ ^= true;
    if (res.negative_ && blocks_.size() == 1 && blocks_[0].number == 0)
        res.negative_ = false;
    return res;
}

BigInt BigInt::operator+(const BigInt& rhs) const {
    if (negative_ != rhs.negative_)
        return *this - (-rhs);

    BigInt Sum;
    Sum.negative_ = negative_;

    block_type carry = 0;

    auto add_block = [&] (block_type lhs, block_type rhs) {
        Block sum(lhs + rhs + carry);
        carry = sum.number / _BASE_;
        sum.number %= _BASE_;
        return sum;
    };

    auto [min, max] = std::minmax(blocks_.size(), rhs.blocks_.size());
    for (size_t i = 0; i < min; i++) {
        Sum.blocks_.push_back(add_block(blocks_[i].number, rhs.blocks_[i].number));
    }

    auto& max_cont = (max == blocks_.size()) ? blocks_ : rhs.blocks_;
    for (size_t i = min; i < max; i++) {
        Sum.blocks_.push_back(add_block(max_cont[i].number, 0));
    }

    Sum.blocks_.push_back(Block(carry));
    Sum.remove_leading_zeros();
    return Sum;
}

BigInt BigInt::operator-(const BigInt& rhs) const {
    if (negative_ != rhs.negative_)
        return *this + (-rhs);

    auto abs = [] (const BigInt& bi) {
        return (bi < 0) ? -bi : bi;
    };

    bool abs_greater = (abs(*this) < abs(rhs));
    auto& reduced = abs_greater ? rhs : *this;
    auto& subtracted = abs_greater ? *this : rhs;

    BigInt Diff(reduced);
    if (abs_greater)
        Diff.negative_ ^= true;

    block_type debt = 0;

    auto subtract_block = [&] (block_type lhs, block_type rhs) {
        if (lhs < debt + rhs) {
            lhs += _BASE_ - debt;
            debt = 1;
        } else {
            lhs -= debt;
            debt = 0;
}
        return Block(lhs - rhs);
    };

    for (size_t i = 0; i < subtracted.blocks_.size(); ++i)
        Diff.blocks_[i] = subtract_block(reduced.blocks_[i].number,
                                         subtracted.blocks_[i].number);
    for (size_t i = subtracted.blocks_.size(); i < reduced.blocks_.size(); ++i)
        Diff.blocks_[i] = subtract_block(reduced.blocks_[i].number, 0);

    Diff.remove_leading_zeros();
    return Diff;
}

BigInt BigInt::operator*(const BigInt&) const {
    return *this;
}

BigInt& BigInt::operator+=(const BigInt& other) { return *this = *this + other; }
BigInt& BigInt::operator-=(const BigInt& other) { return *this = *this - other; }
BigInt& BigInt::operator*=(const BigInt& other) { return *this = *this * other; }
