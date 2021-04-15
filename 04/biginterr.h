#ifndef BIGINT_ERR_H
#define BIGINT_ERR_H

#include <stdexcept>

class BigIntError : public std::runtime_error {
 public:
    using std::runtime_error::runtime_error;
};

class ParsingError : public BigIntError {
 public:
    using BigIntError::BigIntError;
};


#endif  // BIGINT_ERR_H
