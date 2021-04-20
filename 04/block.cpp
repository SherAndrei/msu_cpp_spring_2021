#include "block.h"

std::string Block::to_string(bool add_zeros) const {
    std::string result = {};
    std::string str_num = std::to_string(number);
    size_t digits_in_block = str_num.length();
    if (add_zeros && digits_in_block < _BASE_NDIGITS_) {
        result += std::string(_BASE_NDIGITS_ - digits_in_block, '0');
    }
    return result + str_num;
}
