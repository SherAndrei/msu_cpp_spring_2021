#include "block.h"

std::string Block::to_string(bool add_zeros) const {
    std::string result = {};
    size_t digits_in_block = ndigits();
    if (add_zeros && digits_in_block < _BASE_NDIGITS_) {
        result += std::string(_BASE_NDIGITS_ - digits_in_block, '0');
    }
    return result + std::to_string(number);
}

size_t Block::ndigits() const {
    return std::to_string(number).length();
}
