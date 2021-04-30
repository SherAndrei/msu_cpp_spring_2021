#ifndef BIGINT_H
#define BIGINT_H

#include <concepts>
#include <compare>
#include <iostream>
#include <string>
#include <string_view>

#include "Vector.h"
#include "block.h"

class BigInt {
 public:
    BigInt() = default;

    explicit BigInt(std::string_view sv);
    BigInt& operator=(std::string_view sv);

    explicit BigInt(std::integral auto num);
    BigInt& operator=(std::integral auto num);

 public:
    std::string to_string() const;
    void swap(BigInt&);

 public:
    friend bool operator  <(const BigInt& lhs, const BigInt& rhs);
    friend bool operator ==(const BigInt& lhs, const BigInt& rhs);
    friend auto operator<=>(const BigInt& lhs, const BigInt& rhs) = default;

    BigInt operator-() const;

    BigInt& operator++();
    BigInt& operator--();

    BigInt operator++(int) const;
    BigInt operator--(int) const;

    BigInt& operator+=(const BigInt&);
    BigInt& operator-=(const BigInt&);
    BigInt& operator*=(const BigInt&);

 public:
    BigInt& operator+=(std::integral auto num) { return *this += BigInt(num); }
    BigInt& operator-=(std::integral auto num) { return *this -= BigInt(num); }
    BigInt& operator*=(std::integral auto num) { return *this *= BigInt(num); }

 private:
    void remove_leading_zeros();
    bool is_zero() const;

 private:
    bool negative_ = false;
    Vector<Block> blocks_;
};

std::istream& operator>>(std::istream&, BigInt&);
std::ostream& operator<<(std::ostream&, const BigInt&);

BigInt operator+(const BigInt& lhs, const BigInt& rhs);
BigInt operator-(const BigInt& lhs, const BigInt& rhs);
BigInt operator*(const BigInt& lhs, const BigInt& rhs);

BigInt operator+(const BigInt& lhs, std::integral auto num) { return lhs + BigInt(num); }
BigInt operator-(const BigInt& lhs, std::integral auto num) { return lhs - BigInt(num); }
BigInt operator*(const BigInt& lhs, std::integral auto num) { return lhs * BigInt(num); }

BigInt operator+(std::integral auto num, const BigInt& rhs) { return BigInt(num) + rhs; }
BigInt operator-(std::integral auto num, const BigInt& rhs) { return BigInt(num) - rhs; }
BigInt operator*(std::integral auto num, const BigInt& rhs) { return BigInt(num) * rhs; }

bool operator  <(const BigInt& lhs, std::integral auto num) { return lhs   < BigInt(num); }
bool operator ==(const BigInt& lhs, std::integral auto num) { return lhs  == BigInt(num); }
auto operator<=>(const BigInt& lhs, std::integral auto num) { return lhs <=> BigInt(num); }

bool operator  <(std::integral auto num, const BigInt& rhs) { return BigInt(num)   < rhs; }
bool operator ==(std::integral auto num, const BigInt& rhs) { return BigInt(num)  == rhs; }
auto operator<=>(std::integral auto num, const BigInt& rhs) { return BigInt(num) <=> rhs; }

BigInt::BigInt(std::integral auto num) {
    *this = BigInt(std::to_string(num));
}

BigInt& BigInt::operator=(std::integral auto num) {
    return *this = BigInt(num);
}

#endif  // BIGINT_H
