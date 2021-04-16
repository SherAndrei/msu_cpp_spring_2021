#ifndef BLOCK_H
#define BLOCK_H

#include <string>

using block_type = uint64_t;
constexpr block_type _BASE_ = 10'000'000;
constexpr block_type _BASE_NDIGITS_ = 8;

struct Block {
    block_type number;

    std::string to_string(bool add_zeros = false) const;
    size_t ndigits() const;
};

#endif  // BLOCK_H
