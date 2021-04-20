#ifndef BIGINT_H
#define BIGINT_H

#include <compare>
#include <iostream>
#include <string>
#include <string_view>
#include <memory>

#include "simple_vector.h"
#include "block.h"

class BigInt;

template <class T>
concept IntOrBigInt = std::is_integral_v<T> || std::is_same_v<T, const BigInt&>;

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
    constexpr auto operator<=>(IntOrBigInt auto) const;

    constexpr BigInt operator+(IntOrBigInt auto) const;
    constexpr BigInt operator-(IntOrBigInt auto) const;
    constexpr BigInt operator*(IntOrBigInt auto) const;

    constexpr BigInt& operator+=(IntOrBigInt auto);
    constexpr BigInt& operator-=(IntOrBigInt auto);
    constexpr BigInt& operator*=(IntOrBigInt auto);

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
