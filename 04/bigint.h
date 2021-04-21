#ifndef BIGINT_H
#define BIGINT_H

#include <compare>
#include <iostream>
#include <string>
#include <string_view>

#include "simple_vector.h"
#include "block.h"

template <class T>
concept Integral = std::is_integral_v<T>;

class BigInt {
 public:
    BigInt() = default;

    explicit BigInt(std::string_view sv);
    BigInt& operator=(std::string_view sv);

    BigInt(const BigInt&) = default;
    BigInt& operator=(const BigInt&) = default;

    explicit BigInt(Integral auto num);
    BigInt& operator=(Integral auto num);

    BigInt(BigInt&&) = default;
    BigInt& operator=(BigInt&&) = default;

    ~BigInt() = default;

 public:
    std::string to_string() const;

 public:
    bool operator  <(const BigInt&) const;
    bool operator ==(const BigInt&) const;
    auto operator<=>(const BigInt&) const = default;

    BigInt operator-() const;

    BigInt operator+(const BigInt&) const;
    BigInt operator-(const BigInt&) const;
    BigInt operator*(const BigInt&) const;

    BigInt& operator+=(const BigInt&);
    BigInt& operator-=(const BigInt&);
    BigInt& operator*=(const BigInt&);

 public:
    bool operator  <(Integral auto num) const { return *this   < BigInt(num); }
    bool operator ==(Integral auto num) const { return *this  == BigInt(num); }
    auto operator<=>(Integral auto num) const { return *this <=> BigInt(num); }

    BigInt operator+(Integral auto num) const { return *this + BigInt(num); }
    BigInt operator-(Integral auto num) const { return *this - BigInt(num); }
    BigInt operator*(Integral auto num) const { return *this * BigInt(num); }

    BigInt& operator+=(Integral auto num) { return *this = *this + BigInt(num); }
    BigInt& operator-=(Integral auto num) { return *this = *this - BigInt(num); }
    BigInt& operator*=(Integral auto num) { return *this = *this * BigInt(num); }

 private:
    void remove_leading_zeros();
    bool is_zero() const;

 private:
    bool negative_ = false;
    SimpleVector<Block> blocks_;
};

std::istream& operator>>(std::istream&, BigInt&);
std::ostream& operator<<(std::ostream&, const BigInt&);

BigInt::BigInt(Integral auto num) {
    *this = BigInt(std::to_string(num));
}

BigInt& BigInt::operator=(Integral auto num) {
    return *this = BigInt(std::to_string(num));
}

#endif  // BIGINT_H
