#ifndef BLOCK_H
#define BLOCK_H

#include <string>

using block_type = uint64_t;
constexpr block_type _BASE_ = 100'000'000;
constexpr size_t _BASE_NDIGITS_ = 8;

struct Block {
    block_type number;

    bool operator==(const Block& rhs) const { return number == rhs.number; }
    bool operator< (const Block& rhs) const { return number <  rhs.number; }
    std::string to_string(bool add_zeros = false) const;
};

#endif  // BLOCK_H
