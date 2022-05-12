#ifndef BIGINT_H
#define BIGINT_H

#include <iostream>
#include <string>
#include <string_view>

#include "Vector.h"
#include "block.h"

class BigInt final {
 public:
    BigInt(std::string_view sv);
    BigInt& operator=(std::string_view sv);

    template <typename integral, typename = std::enable_if_t<std::is_integral_v<integral>>>
    BigInt(integral num) { *this = BigInt(std::to_string(num)); }
    template <typename integral, typename = std::enable_if_t<std::is_integral_v<integral>>>
    BigInt& operator=(integral num) { return *this = BigInt(num); }

 public:
    std::string to_string() const;
    void swap(BigInt&) noexcept;

 public:
    friend bool operator==(const BigInt& lhs, const BigInt& rhs);
    friend bool operator!=(const BigInt& lhs, const BigInt& rhs);
    friend bool operator <(const BigInt& lhs, const BigInt& rhs);
    friend bool operator >(const BigInt& lhs, const BigInt& rhs);
    friend bool operator<=(const BigInt& lhs, const BigInt& rhs);
    friend bool operator>=(const BigInt& lhs, const BigInt& rhs);

    BigInt operator-() const;

    BigInt& operator++();
    BigInt& operator--();

    BigInt operator++(int);
    BigInt operator--(int);

    BigInt& operator+=(const BigInt&);
    BigInt& operator-=(const BigInt&);
    BigInt& operator*=(const BigInt&);

 public:
    template <typename integral, typename = std::enable_if_t<std::is_integral_v<integral>>>
    BigInt& operator+=(integral num) { return *this += BigInt(num); }
    template <typename integral, typename = std::enable_if_t<std::is_integral_v<integral>>>
    BigInt& operator-=(integral num) { return *this -= BigInt(num); }
    template <typename integral, typename = std::enable_if_t<std::is_integral_v<integral>>>
    BigInt& operator*=(integral num) { return *this *= BigInt(num); }

 private:
    BigInt(bool neg, Vector<Block>&& blocks) noexcept;
    void remove_leading_zeros();
    bool is_zero() const noexcept;

 private:
    bool negative_ = false;
    Vector<Block> blocks_;
};

std::istream& operator>>(std::istream&, BigInt&);
std::ostream& operator<<(std::ostream&, const BigInt&);

BigInt operator+(const BigInt& lhs, const BigInt& rhs);
BigInt operator-(const BigInt& lhs, const BigInt& rhs);
BigInt operator*(const BigInt& lhs, const BigInt& rhs);

template <typename integral, typename = std::enable_if_t<std::is_integral_v<integral>>>
BigInt operator+(const BigInt& lhs, integral num) { return lhs + BigInt(num); }
template <typename integral, typename = std::enable_if_t<std::is_integral_v<integral>>>
BigInt operator-(const BigInt& lhs, integral num) { return lhs - BigInt(num); }
template <typename integral, typename = std::enable_if_t<std::is_integral_v<integral>>>
BigInt operator*(const BigInt& lhs, integral num) { return lhs * BigInt(num); }

template <typename integral, typename = std::enable_if_t<std::is_integral_v<integral>>>
BigInt operator+(integral num, const BigInt& rhs) { return BigInt(num) + rhs; }
template <typename integral, typename = std::enable_if_t<std::is_integral_v<integral>>>
BigInt operator-(integral num, const BigInt& rhs) { return BigInt(num) - rhs; }
template <typename integral, typename = std::enable_if_t<std::is_integral_v<integral>>>
BigInt operator*(integral num, const BigInt& rhs) { return BigInt(num) * rhs; }

template <typename integral, typename = std::enable_if_t<std::is_integral_v<integral>>>
bool operator==(const BigInt& lhs, integral  num) { return lhs == BigInt(num); }
template <typename integral, typename = std::enable_if_t<std::is_integral_v<integral>>>
bool operator!=(const BigInt& lhs, integral num) { return lhs != BigInt(num); }
template <typename integral, typename = std::enable_if_t<std::is_integral_v<integral>>>
bool operator <(const BigInt& lhs, integral num) { return lhs  < BigInt(num); }
template <typename integral, typename = std::enable_if_t<std::is_integral_v<integral>>>
bool operator >(const BigInt& lhs, integral num) { return lhs  > BigInt(num); }
template <typename integral, typename = std::enable_if_t<std::is_integral_v<integral>>>
bool operator<=(const BigInt& lhs, integral num) { return lhs <= BigInt(num); }
template <typename integral, typename = std::enable_if_t<std::is_integral_v<integral>>>
bool operator>=(const BigInt& lhs, integral num) { return lhs >= BigInt(num); }

template <typename integral, typename = std::enable_if_t<std::is_integral_v<integral>>>
bool operator==(integral num, const BigInt& rhs) { return BigInt(num) == rhs; }
template <typename integral, typename = std::enable_if_t<std::is_integral_v<integral>>>
bool operator!=(integral num, const BigInt& rhs) { return BigInt(num) != rhs; }
template <typename integral, typename = std::enable_if_t<std::is_integral_v<integral>>>
bool operator <(integral num, const BigInt& rhs) { return BigInt(num)  < rhs; }
template <typename integral, typename = std::enable_if_t<std::is_integral_v<integral>>>
bool operator >(integral num, const BigInt& rhs) { return BigInt(num)  > rhs; }
template <typename integral, typename = std::enable_if_t<std::is_integral_v<integral>>>
bool operator<=(integral num, const BigInt& rhs) { return BigInt(num) <= rhs; }
template <typename integral, typename = std::enable_if_t<std::is_integral_v<integral>>>
bool operator>=(integral num, const BigInt& rhs) { return BigInt(num) >= rhs; }

#endif  // BIGINT_H
