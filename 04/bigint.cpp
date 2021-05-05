#include <iostream>
#include <string>
#include <utility>
#include <algorithm>
#include <charconv>
#include <ranges>

#include "bigint.h"
#include "biginterr.h"

namespace {

void LeftStrip(std::string_view& sv) {
    while (!sv.empty() && std::isspace(sv[0]))
        sv.remove_prefix(1);
}

std::string_view ReadToken(std::string_view& sv) {
    LeftStrip(sv);
    auto pos = sv.find_first_of(" \t\n");
    auto result = sv.substr(0, pos);
    sv.remove_prefix(pos != sv.npos ? pos : sv.size());
    return result;
}

bool IsEndOfToken(char ch) {
    return (ch == '\0' || std::isdigit(ch));
}

void Parse(std::string_view sv, Vector<Block>& data) {
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
    LeftStrip(sv);
    if (sv[0] == '-') {
        negative_ = true;
        sv.remove_prefix(1);
    }
    while (!sv.empty()) {
        size_t min_size = std::min(_BASE_NDIGITS_, sv.size());
        Parse(sv.substr(sv.size() - min_size), blocks_);
        sv.remove_suffix(min_size);
    }
    if (is_zero()) negative_ = false;

    remove_leading_zeros();
}

BigInt& BigInt::operator=(std::string_view sv) {
    return *this = BigInt(sv);
}

BigInt operator+(const BigInt& lhs, const BigInt& rhs) { return BigInt(lhs) += rhs; }
BigInt operator-(const BigInt& lhs, const BigInt& rhs) { return BigInt(lhs) -= rhs; }
BigInt operator*(const BigInt& lhs, const BigInt& rhs) { return BigInt(lhs) *= rhs; }

namespace rng = std::ranges;

std::string BigInt::to_string() const {
    std::string res;
    res.reserve(blocks_.size() * _BASE_NDIGITS_);
    if (negative_) res += '-';
    bool add_zeros = false;
    rng::for_each(rng::reverse_view(blocks_), [&res, &add_zeros] (Block bl) {
        res += bl.to_string(add_zeros);
        add_zeros = true;
    });

    return res;
}

void BigInt::swap(BigInt& other) {
    blocks_.swap(other.blocks_);
    std::swap(negative_, other.negative_);
}

void BigInt::remove_leading_zeros() {
    while (blocks_.size() > 1 && blocks_.back().number == 0)
        blocks_.pop_back();
}

bool BigInt::is_zero() const {
    return blocks_.size() == 1 && blocks_[0].number == 0;
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

bool operator < (const BigInt& lhs, const BigInt& rhs) {
    return rhs.negative_ <= lhs.negative_ &&
           rng::lexicographical_compare(
               rng::reverse_view(lhs.blocks_),
               rng::reverse_view(rhs.blocks_),
               [&] (Block lhsb, Block rhsb) {
                   return (lhsb.number < rhsb.number) ^ lhs.negative_;
           });
}

bool operator == (const BigInt& lhs, const BigInt& rhs) {
    return std::tie(lhs.negative_, lhs.blocks_) == std::tie(rhs.negative_, rhs.blocks_);
}

BigInt BigInt::operator-() const {
    BigInt res{*this};
    res.negative_ ^= true;
    if (res.is_zero()) res.negative_ = false;
    return res;
}

BigInt& BigInt::operator++() { return *this += 1; }
BigInt& BigInt::operator--() { return *this -= 1; }

BigInt BigInt::operator++(int) const { return *this + 1; }
BigInt BigInt::operator--(int) const { return *this - 1; }

BigInt& BigInt::operator+=(const BigInt& rhs) {
    if (negative_ != rhs.negative_)
        return *this -= (-rhs);

    BigInt Sum;
    Sum.negative_ = negative_;

    block_type carry = 0;

    auto add_block = [&carry] (block_type lhs, block_type rhs) {
        block_type sum = lhs + rhs + carry;
        carry = sum / _BASE_;
        sum %= _BASE_;
        return sum;
    };

    auto [min, max] = std::minmax({ blocks_.size(), rhs.blocks_.size() });
    for (size_t i = 0; i < min; i++)
        Sum.blocks_.emplace_back(add_block(blocks_[i].number, rhs.blocks_[i].number));

    auto& max_cont = (max == blocks_.size()) ? blocks_ : rhs.blocks_;
    for (size_t i = min; i < max; i++)
        Sum.blocks_.emplace_back(add_block(max_cont[i].number, 0));

    Sum.blocks_.emplace_back(carry);

    Sum.remove_leading_zeros();
    this->swap(Sum);
    return *this;
}
BigInt& BigInt::operator-=(const BigInt& rhs) {
    if (negative_ != rhs.negative_)
        return *this += (-rhs);

    auto abs = [] (const BigInt& bi) {
        return (bi < 0) ? -bi : bi;
    };

    bool abs_greater = (abs(*this) < abs(rhs));
    auto& reduced = abs_greater ? rhs : *this;
    auto& subtracted = abs_greater ? *this : rhs;

    BigInt Diff;
    Diff.negative_ = reduced.negative_;
    Diff.blocks_.reserve(reduced.blocks_.size());

    if (abs_greater)
        Diff.negative_ ^= true;

    block_type debt = 0;

    auto subtract_block = [&debt] (block_type lhs, block_type rhs) {
        if (lhs < debt + rhs) {
            lhs += _BASE_ - debt;
            debt = 1;
        } else {
            lhs -= debt;
            debt = 0;
        }
        return lhs - rhs;
    };

    for (size_t i = 0; i < subtracted.blocks_.size(); ++i)
        Diff.blocks_.emplace_back(subtract_block(reduced.blocks_[i].number, subtracted.blocks_[i].number));

    for (size_t i = subtracted.blocks_.size(); i < reduced.blocks_.size(); ++i)
        Diff.blocks_.emplace_back(subtract_block(reduced.blocks_[i].number, 0));

    Diff.remove_leading_zeros();
    this->swap(Diff);
    return *this;
}
BigInt& BigInt::operator*=(const BigInt& rhs) {
    BigInt Prod;
    Prod.negative_ = negative_ ^ rhs.negative_;
    Prod.blocks_.resize(blocks_.size() + rhs.blocks_.size());

    for (size_t i = 0; i < blocks_.size(); ++i) {
        block_type carry = 0;
        for (size_t j = 0; j < rhs.blocks_.size(); ++j) {
            uint64_t prod_item = blocks_[i].number * rhs.blocks_[j].number
                               + Prod.blocks_[i + j].number + carry;
            Prod.blocks_[i + j].number = prod_item % _BASE_;
            carry = prod_item / _BASE_;
        }
        Prod.blocks_[i + rhs.blocks_.size()].number += carry;
    }
    Prod.remove_leading_zeros();
    if (Prod.is_zero()) Prod.negative_ = false;
    this->swap(Prod);
    return *this;
}
