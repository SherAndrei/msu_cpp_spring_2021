#ifndef BIGINT_H
#define BIGINT_H

#include <compare>
#include <iostream>
#include <string>
#include <string_view>
#include <memory>

#include "simple_vector.h"

class BigInt;

template <class T>
concept Integral = std::is_integral_v<T> || std::is_same_v<T, const BigInt&>;

constexpr uint64_t _BASE_ = 10'000'000;
constexpr uint64_t _BASE_NDIGITS_ = 8;

// TODO
class Block {
 private:
    size_t number;
};


class BigInt {
 public:
    BigInt() = default;
    explicit BigInt(std::string_view sv);
    BigInt& operator=(std::string_view sv);

    explicit BigInt(Integral auto num);
    BigInt& operator=(Integral auto num);

    BigInt(BigInt&&) = default;
    BigInt& operator=(BigInt&&) = default;

    ~BigInt() = default;

 public:
    size_t ndigits() const;

 public:
    std::string to_string() const;

 public:
    constexpr auto operator<=>(Integral auto) const;

    constexpr BigInt operator+(Integral auto) const;
    constexpr BigInt operator-(Integral auto) const;
    constexpr BigInt operator*(Integral auto) const;
    constexpr BigInt operator/(Integral auto) const;
    constexpr BigInt operator%(Integral auto) const;

    constexpr BigInt& operator+=(Integral auto);
    constexpr BigInt& operator-=(Integral auto);
    constexpr BigInt& operator*=(Integral auto);
    constexpr BigInt& operator/=(Integral auto);
    constexpr BigInt& operator%=(Integral auto);

    const BigInt& operator++();
    const BigInt& operator--();
    BigInt operator++(int);
    BigInt operator--(int);

 private:
    bool sign_ = false;
    SimpleVector<uint64_t> data_;
};

std::istream& operator>>(std::istream&, BigInt&);
std::ostream& operator<<(std::ostream&, const BigInt&);

#endif  // BIGINT_H