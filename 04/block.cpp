#include "block.h"

std::string Block::to_string(bool add_zeros) const {
    std::string str_num = std::to_string(number);
    size_t digits_in_block = str_num.length();
    if (add_zeros && digits_in_block < _BASE_NDIGITS_)
        return std::string(_BASE_NDIGITS_ - digits_in_block, '0') + str_num;
    return str_num;
}
