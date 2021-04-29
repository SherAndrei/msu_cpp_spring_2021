#ifndef FORMAT_ERROR_H
#define FORMAT_ERROR_H

#include <exception>

struct FormatError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct BracesError : public FormatError {
    using FormatError::FormatError;
};

struct ArgumentError : public FormatError {
    using FormatError::FormatError;
};

#endif  // FORMAT_ERROR_H
